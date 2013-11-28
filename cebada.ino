#include <EEPROM.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_SR.h>
#include "Control.h"
#include "System.h"


#define s_OUT_CLOCK	6
#define s_OUT_DATA      5
#define s_OUT_ENABLE    7
#define s_ONE_WIRE_BUS  A4

#define s_LCD_ENABLE	2
#define s_LCD_CLOCK	3
#define s_LCD_DATA	4

#define s_IN_UP		A0
#define s_IN_DOWN	A1
#define s_IN_SET	A2
#define s_IN_BACK	A3

System system(
            s_ONE_WIRE_BUS,
            s_OUT_ENABLE,
            s_OUT_CLOCK,
            s_OUT_DATA,
            s_LCD_ENABLE,
            s_LCD_CLOCK,
            s_LCD_DATA,
            s_IN_UP,
            s_IN_DOWN,
            s_IN_SET,
            s_IN_BACK
);

unsigned long time;

void setup()
{
   system.begin();
}

void loop()
{
  
   system.read_inputs();
   
   if (millis() - time >1000){
     system.temp_processing();
     time = millis(); 
   }
  
  
}




