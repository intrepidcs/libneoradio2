#include "neoRAD-IO2.h"
#include "Arduino.h"

neoRADIO2_DeviceInfo* deviceInterfaceInt;

uint8_t neoRADIO2CRC8Table[256];

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

int neoRADIO2IdentifyChain(neoRADIO2_DeviceInfo * deviceInfo)
{
    uint8_t buf[3];

    buf[0] = NEORADIO2_DEVTYPE_HOST; //chip
    buf[1] = 0xFF; //id
    buf[2] = 0xFF; //id

    if (neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_IDENTIFY, 0xFF, 0xFF, buf, sizeof(buf), 0) == 0)
    {
        deviceInfo->State = neoRADIO2state_ConnectedWaitIdentResponse;
        deviceInfo->LastBank = 0;
        deviceInfo->LastDevice = 0;
        return 0;
    }
    else
    {
        return -1;
    }
}

int neoRADIO2SendJumpToApp(neoRADIO2_DeviceInfo * deviceInfo)
{
  if(neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_START, 0xFF, 0xFF, NULL, 0, 0))
  {
    deviceInfo->State = neoRADIO2state_ConnectedWaitForAppStart;
    deviceInfo->isOnline = true;
  }
}

int neoRADIO2SendPacket(neoRADIO2_DeviceInfo * devInfo, uint8_t command, uint8_t device, uint8_t bank, uint8_t * data, uint8_t len, uint8_t blocking)
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
    return pushUart(&devInfo, buf, txlen);
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

//                if(deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].header.command_status == NEORADIO2_STATUS_SENSOR)
//                {
//                  bytesToFloat temp;
//                  memcpy(temp.b, deviceInfo->rxDataBuffer[deviceInfo->rxDataCount].data, sizeof(temp));
//                  deviceInfo->bankSensor[header->bank].data = temp.fp;
//                }
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

int neoRADIO2ProcessIncomingData(neoRADIO2_DeviceInfo * devInfo, uint64_t diffTimeus)
{
    int result = 0;
    devInfo->Timeus += diffTimeus;
    if (devInfo->State != neoRADIO2state_Disconnected)
    {
         neoRADIO2LookForDevicePackets(devInfo);
    }
    switch (devInfo->State)
    {
        case neoRADIO2state_Disconnected:
            break;
        case neoRADIO2state_ConnectedWaitForAppStart:
            neoRADIO2LookForStartHeader(devInfo);
            break;
        case neoRADIO2state_Connected:
            if (devInfo->isOnline)
               neoRADIO2GetSensorData(devInfo);
            break;
        default:
            break;
    }
    return result;
}

void neoRADIO2LookForStartHeader(neoRADIO2_DeviceInfo * deviceInfo)
{
    for (int i = 0; i < deviceInfo->rxDataCount; i++)
    {
      if(deviceInfo->rxDataBuffer[i].header.start_of_frame == 0xAA && \
      deviceInfo->rxDataBuffer[i].header.command_status == NEORADIO2_COMMAND_START)
      {
        deviceInfo->State = neoRADIO2state_Connected;
      }
    }
}

void neoRADIO2GetSensorData(neoRADIO2_DeviceInfo* deviceInfo)
{
  for ( int i = 0; i < deviceInfo->rxDataCount; i++)
  {
    if(deviceInfo->rxDataBuffer[i].header.start_of_frame == 0x55 && \
      deviceInfo->rxDataBuffer[i].header.command_status == NEORADIO2_STATUS_SENSOR)
      {
           bytesToFloat temp;
           memcpy(temp.b, deviceInfo->rxDataBuffer[i].data, sizeof(temp));
           deviceInfo->bankSensor[i].data = temp.fp;
      }
  }
}

void neoRADIO2ToggleLED(neoRADIO2_DeviceInfo* deviceInfo, uint8_t device, uint8_t bank, int ms)
{
  uint8_t buf[2];
  buf[0] = ms & 0xFF;
  neoRADIO2SendPacket(deviceInfo, NEORADIO2_COMMAND_TOGGLE_LED, device, bank, buf, 1, 0);
}

int pushUart(neoRADIO2_DeviceInfo * deviceInfo, uint8_t* data, int len)
{
    cli();
    if(FIFO_GetFreeSpace(&deviceInterfaceInt->txfifo) < len)
      return -1;
    FIFO_Push(&deviceInterfaceInt->txfifo, data, len);
    UDR0 = FIFO_PopOne(&deviceInterfaceInt->txfifo);
    sei();
    return 1;
}

ISR(USART_TX_vect) //Interrupt to add UART Rx to the FIFO
{
  if (FIFO_GetCount(&deviceInterfaceInt->txfifo))
    UDR0 = FIFO_PopOne(&deviceInterfaceInt->txfifo);
}

ISR(USART_RX_vect) //Interrupt to send UART Tx from the FIFO
{
  if(FIFO_GetFreeSpace(&deviceInterfaceInt->rxfifo))
    FIFO_PushOne(&deviceInterfaceInt->rxfifo, UDR0);
  
}

ISR(TIMER2_COMPA_vect) {
    neoRADIO2ProcessIncomingData(&deviceInterfaceInt, 1000); // Must be called every 1ms 
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

int neoRADIO2InitInterfaceHardware(neoRADIO2_DeviceInfo* deviceInfo)
{
  //setup for Device controlled interrupts
  deviceInterfaceInt = deviceInfo;
  
  cli(); // Turn off interupts
  
  //Configure Timer 2
  TCCR2A = 0;
  TCCR2B = 0;
  TCNT2  = 0;
  OCR2A = 1999;
  TCCR2B |= (1 << WGM12);
  TCCR2B |= (1 << CS11);  
  TIMSK2 |= (1 << OCIE2A);

  //Setting up UART dev neoRAD-IO devices operate UART on 250000 baudrate
  SREG = 0x80;
  UBRR0=3; // (FREQ / Desired baudrate
  UCSR0C |= (1<< UCSZ01) | (1 << UCSZ00);
  UCSR0B |= (1<<RXEN0) | (1 << TXEN0) | (1 << RXCIE0) | (1 << TXCIE0);

  //Now lets enable 
  sei();

  //Enabling Fifo's for handling neoRAD-IO communication (This eats the arduino's SRAM)
  FIFO_Init(&deviceInfo->rxfifo, deviceInfo->rxbuffer, NEORADIO2_RX_BUFFER_SIZE);
  FIFO_Init(&deviceInfo->txfifo, deviceInfo->txbuffer, NEORADIO2_TX_BUFFER_SIZE);

  //Lets init our crc table
  neoRADIO2CRC8_Init();

}
