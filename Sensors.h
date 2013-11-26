#ifndef Sensors_h
#define Sensors_h

#include "Arduino.h"
#include <OneWire.h>
#include <DallasTemperature.h>

class Sensors
{
  private:
  
    OneWire oneWire;
    DallasTemperature ds_sensors;
    byte allAddress [4][8];
    unsigned char totalDevices;
    const unsigned char maxNumberOfDevices;
    const unsigned char resolution; //bits
    
  public:
   Sensors(int ONE_WIRE_BUS);
   void begin();
   byte DiscoverOneWireDevices();
   byte DiscoverOneWireDevices(byte from_index);
   void requestTemperatures();
   void reset_search();
   float getTemperature(DeviceAddress addr);
   unsigned char getTotalDevices();
   byte getDeviceAddress(byte *newAddr, byte index);
   void printAddress(byte *sensor_id);
};

#endif
