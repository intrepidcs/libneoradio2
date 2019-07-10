#define RADIO2_EMBEDDED_LIB

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#include "fifo.h"
#include "neoRAD-IO2.h"
#include "neoRAD-IO2-TC.h"
#include "neoRAD-IO2_PacketHandler.h"
#include "neoRADIO2_frames.h"


// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

// Define proper RST_PIN if required.
#define RST_PIN -1

SSD1306AsciiWire oled;


uint8_t neoRADIO2CRC8Table[256];

int neoRADIO2IdentifyChain(neoRADIO2_DeviceInfo * deviceInfo)
{
    uint8_t buf[3];

    buf[0] = NEORADIO2_DEVTYPE_HOST; //chip
    buf[1] = 0xFF; //id
    buf[2] = 0xFF; //id

    if (neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_IDENTIFY, 0xFF, 0xFF, buf, sizeof(buf)) == 0)
    {
        deviceInfo->State = neoRADIO2state_ConnectedWaitIdentResponse;
        deviceInfo->LastBank = 0;
        deviceInfo->LastDevice = 0;
       // memset(deviceInfo->ChainList, 0x00, sizeof(deviceInfo->ChainList));
        return 0;
    }
    else
    {
        return -1;
    }
}

int neoRADIO2SendJumpToApp(neoRADIO2_DeviceInfo * deviceInfo)
{
    if (neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_START, 0xFF, 0xFF, NULL, 0) == 0)
    {
        deviceInfo->State = neoRADIO2state_ConnectedWaitForAppStart;
        return 0;
    }
    else
    {
        return -1;
    }
}

int neoRADIO2SendPacket(neoRADIO2_DeviceInfo * devInfo, uint8_t command, uint8_t device, uint8_t bank, uint8_t * data, uint8_t len)
{
    unsigned int txlen = sizeof(neoRADIO2frame_header) + len + 1; //extra byte for crc
    uint8_t buf[16];
    neoRADIO2frame_header * header = (neoRADIO2frame_header *)buf;

  if (len > (sizeof(buf) - sizeof(neoRADIO2frame_header)))
        return -1;
        
    header->start_of_frame = 0xAA;
    header->command_status = command;
    header->device = device;
    header->bank = bank;
    header->len = len;

    for (int i = 0; i < len; i++)
    {
        buf[i + sizeof(neoRADIO2frame_header)] = data[i];
    }
    buf[txlen - 1] = neoRADIO2CalcCRC8(buf, txlen - 1);

    //FIFO
    return pushUart(buf, txlen);
}

void neoRADIO2LookForDevicePackets(neoRADIO2_DeviceInfo * deviceInfo)
{
    unsigned int len = 0;
    unsigned int loc = 0;
    unsigned int readsize;
    static const unsigned int minPacketSize = sizeof(neoRADIO2frame_header) + 1; //extra byte for CRC
    neoRADIO2frame_header  * header;
    uint8_t rxdata[NEORADIO2_RX_BUFFER_SIZE];

    deviceInfo->rxDataCount = 0;
    len = FIFO_GetCount(&deviceInfo->rxfifo);
    if(len > sizeof(rxdata))
        len = sizeof(rxdata);

    if (len == 0)
        return;

    if (len < minPacketSize)
        return;

    readsize = FIFO_GetOneShotReadSize(&deviceInfo->rxfifo);
    memcpy(rxdata, FIFO_GetReadPtr(&deviceInfo->rxfifo), readsize);

    if (readsize < len)
    {
        memcpy(&rxdata[readsize], deviceInfo->rxfifo.ptr, len - readsize);
    }
    while ((loc + minPacketSize) <= len)
    {
        if (rxdata[loc] == 0x55 || rxdata[loc] == 0xAA)
        {
          header = (neoRADIO2frame_header *)&rxdata[loc];
            if ((header->len + minPacketSize) > (len - loc))
            {
                return;
            }
          unsigned int dataloc = sizeof(neoRADIO2frame_header) + loc;
            if (neoRADIO2CalcCRC8(&rxdata[loc], sizeof(neoRADIO2frame_header) + header->len) == rxdata[dataloc + header->len])
            {
              deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].header.bank = header->bank;
              deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].header.command_status = header->command_status;
                for (int i = 0; i < header->len; i++)
                {
                    deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].data[i] = rxdata[dataloc + i];
                }

                if(deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].header.command_status == NEORADIO2_STATUS_SENSOR)
                {
                  bytesToFloat temp;
                  memcpy(temp.b, deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].data, sizeof(temp));
                  deviceInfo->bankSensor[header->bank].data = temp.fp;
                }
                deviceInfo->rxDataCount++;
                FIFO_IncrementOutPtr(&deviceInfo->rxfifo, (header->len + minPacketSize));
                loc += (header->len + minPacketSize);
            }
            else
            {
                loc++;
                FIFO_IncrementOutPtr(&deviceInfo->rxfifo, 1);
            }
        }
        else
        {
            loc++;
            FIFO_IncrementOutPtr(&deviceInfo->rxfifo, 1);
        }
    }
}

void neoRADIO2LookForStartHeader(neoRADIO2_DeviceInfo * deviceInfo)
{
    for (int i = 0; i < deviceInfo->rxDataCount; i++)
    {
      if(deviceInfo->rxDataBuffer[i].header.start_of_frame == 0xAA && \
      deviceInfo->rxDataBuffer[i].header.command_status == NEORADIO2_COMMAND_START)
      {
        if (neoRADIO2IdentifyChain(deviceInfo) != 0)
        {
          deviceInfo->State = neoRADIO2state_Disconnected;
        }
      }
    }
}

uint8_t neoRADIO2CalcCRC8(uint8_t * data, int len)
{
  if (neoRADIO2CRC8Table[1] != CRC_POLYNIMIAL)
    neoRADIO2CRC8_Init();
  uint8_t crc = 0;
  for (int i = 0; i < len; i++)
  {
    crc = neoRADIO2CRC8Table[crc ^ data[i]] & 0xFF;
  }
  return crc;

}
void neoRADIO2CRC8_Init(void)
{
  uint8_t crc;

  for (int i = 0; i < 256; i++)
  {
    crc = i;
    for (int j = 0; j < 8; j++)
    {
      crc = (crc << 1) ^ ((crc & 0x80) ? CRC_POLYNIMIAL : 0);
    }
    neoRADIO2CRC8Table[i] = crc & 0xFF;
  }
}

void neoRADIO2ToggleLED(neoRADIO2_DeviceInfo* deviceInfo, uint8_t device, uint8_t bank, int ms)
{
  uint8_t buf[2];

  buf[0] = ms & 0xFF;
  neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_TOGGLE_LED, device, bank, buf, 1);
}

neoRADIO2_DeviceInfo RADdevice;
neoRADIO2_DeviceInfo *pRADdevice = &RADdevice;

void FIFO_Init(fifo_t* f, void* buffer, const unsigned int sz)
{
    f->ptr = (uint8_t*)buffer;
    f->maxSz = sz;
    FIFO_Clear(f);
}

void FIFO_Clear(fifo_t* f)
{
    f->in = f->out = 0;
    f->numItems = 0;
}


void setup() {
   pinMode(LED_BUILTIN, OUTPUT);

   pinMode(12, OUTPUT);

   digitalWrite(12, HIGH);

  FIFO_Init(&pRADdevice->rxfifo, pRADdevice->rxbuffer, NEORADIO2_RX_BUFFER_SIZE);
  FIFO_Init(&pRADdevice->txfifo, pRADdevice->txbuffer, NEORADIO2_TX_BUFFER_SIZE);

 
  Wire.begin();
  Wire.setClock(400000L);

  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.clear();

  cli();//stop interrupts
  //set timer1 interrupt at 1kHz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set timer count for 1khz increments
  OCR1A = 1999;// = (16*10^6) / (1000*8) - 1
  //had to use 16 bit timer1 for this bc 1999>255, but could switch to timers 0 or 2 with larger prescaler
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR1B |= (1 << CS11);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  TCCR2A = 0;// set entire TCCR1A register to 0
  TCCR2B = 0;// same for TCCR1B
  TCNT2  = 0;//initialize counter value to 0
  // set timer count for 1khz increments
  OCR2A = 1999;// = (16*10^6) / (1000*8) - 1
  //had to use 16 bit timer1 for this bc 1999>255, but could switch to timers 0 or 2 with larger prescaler
  // turn on CTC mode
  TCCR2B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR2B |= (1 << CS11);  
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);

  //Setting up UART dev
  SREG = 0x80;
  UBRR0=3;
  UCSR0C |= (1<< UCSZ01) | (1 << UCSZ00);
  UCSR0B |= (1<<RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << TXCIE0);
  sei();

  //Lets init our crc table
  neoRADIO2CRC8_Init();
  delay(100);
  
  neoRADIO2IdentifyChain(pRADdevice);
  neoRADIO2SendJumpToApp(pRADdevice);

  delay(500);
}

int pushUart(uint8_t* data, int len)
{
    cli();
    if(FIFO_GetFreeSpace(&pRADdevice->txfifo) < len)
      return -1;
    FIFO_Push(&pRADdevice->txfifo, data, len);
    UDR0 = FIFO_PopOne(&pRADdevice->txfifo);
    sei();
    return 1;
}

ISR(USART_TX_vect)
{
  if (FIFO_GetCount(&pRADdevice->txfifo))
    UDR0 = FIFO_PopOne(&pRADdevice->txfifo);
}

ISR(USART_RX_vect)
{
  if(FIFO_GetFreeSpace(&pRADdevice->rxfifo))
    FIFO_PushOne(&pRADdevice->rxfifo, UDR0);
  
}

bool newData = false;

uint8_t cntTime = 0;
uint8_t sampleRate = 100;

ISR(TIMER2_COMPA_vect) {
    neoRADIO2LookForDevicePackets(pRADdevice);
    newData = true;
}

ISR(TIMER1_COMPA_vect) {
  
  if(cntTime++ > sampleRate)
    {
      newData = false;
      if(neoRADIO2SendPacket(pRADdevice, NEORADIO2_COMMAND_READ_DATA, 0xFF, 0xFF, NULL, 0) == -1)
      {
        //We are stuck, loop forever
        for(;;)
        {
          digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
          delay(500);
        }
          
      }
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      cntTime = 0;
    }
}

void loop() {
  
  for(int i = 0; i < 8; i++)
  {
    oled.setCursor(0,i);
    oled.print("Channel ");
    oled.print(i+1);
    oled.print(": ");
    if(pRADdevice->bankSensor[i].data != -1000.00 && pRADdevice->bankSensor[i].isConnected)
      oled.println(pRADdevice->bankSensor[i].data);
    else if(pRADdevice->bankSensor[i].data == -1000.00 && pRADdevice->bankSensor[i].isConnected)
    {
       oled.setCursor(65, i);
       oled.println(" N/A  ");
       pRADdevice->bankSensor[i].isConnected = false;
    }
    else
      pRADdevice->bankSensor[i].isConnected = true;
   }
}
 
