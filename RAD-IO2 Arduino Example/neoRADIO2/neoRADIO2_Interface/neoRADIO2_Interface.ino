#define RADIO2_EMBEDDED_LIB

#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

#include "fifo.h"
#include "neoRAD-IO2.h"
#include "neoRAD-IO2-TC.h"
#include "neoRADIO2_frames.h"

#define I2C_ADDRESS 0x3C
#define RST_PIN -1

SSD1306AsciiWire oled;

neoRADIO2_DeviceInfo RADdevice;
neoRADIO2_DeviceInfo *pRADdevice = &RADdevice;

void setup() {

  //Use timer 1 (1ms) for sampling. !n the neoRADIO2 arduino interface, we need to use timer 2 (1ms) to check for packets

  cli();
  
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 1999;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11);  
  TIMSK1 |= (1 << OCIE1A);

  sei();

  //Lets have our ARDUINO LED indicator
  pinMode(LED_BUILTIN, OUTPUT);

  //Configuring I2C for OLED screen
  Wire.begin();
  Wire.setClock(400000L);

  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont(Adafruit5x7);
  oled.clear();

  delay(100);

  neoRADIO2InitInterfaceHardware(pRADdevice);
  
  neoRADIO2IdentifyChain(pRADdevice);
  neoRADIO2SendJumpToApp(pRADdevice);
}

uint8_t cntTime = 0;
uint8_t sampleRate = 100;

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

ISR(TIMER1_COMPA_vect) {
  
  if(cntTime++ > sampleRate && pRADdevice->isOnline) //Controls the sample rate of neoRAD-IO2
    {
      if(neoRADIO2SendPacket(pRADdevice, NEORADIO2_COMMAND_READ_DATA, 0xFF, 0xFF, NULL, 0, 0) == -1)
      {
        //We are stuck, loop forever (UART tx overflow issue)
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
