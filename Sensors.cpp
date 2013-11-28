#include "Arduino.h"
#include "Sensors.h"

Sensors::Sensors(int ONE_WIRE_BUS):
  oneWire(ONE_WIRE_BUS),
  ds_sensors(&oneWire),
  maxNumberOfDevices(4),
  resolution(10)
{
}

void Sensors::begin(){
  ds_sensors.begin();
  
  totalDevices = DiscoverOneWireDevices();
  //for (byte i=0; i < totalDevices; i++) 
  //  ds_sensors.setResolution(allAddress[i], resolution);
}

byte Sensors::DiscoverOneWireDevices() {
  oneWire.reset();
  oneWire.reset_search();
  
  byte from_index = 0;
  
  while ( oneWire.search(allAddress[from_index])) {    
    delay(250);
    from_index++;
  }
  Serial.print("No more addresses.\n");
  //oneWire.reset_search();
  //oneWire.reset();
  return from_index;
}

byte Sensors::DiscoverOneWireDevices(byte from_index) {
  while ((from_index < maxNumberOfDevices) && (oneWire.search(allAddress[from_index]))) {        
    from_index++;
  }
  return from_index;
}

void Sensors::requestTemperatures(){
  ds_sensors.requestTemperatures();
}

void Sensors::reset_search(){
  oneWire.reset_search();
}

byte Sensors::getDeviceAddress(byte *newAddr, byte index){
  
  for (int i = 0; i < 8; i++) newAddr[i] = allAddress[index][i];
  return 1;
}

float Sensors::getTemperature(DeviceAddress addr) {
  float tempC = ds_sensors.getTempC(addr);
  return tempC;  
}

unsigned char Sensors::getTotalDevices(){
  return totalDevices;
}

byte Sensors::checkSensor(DeviceAddress addr){
  return ds_sensors.isConnected(addr);
}

void Sensors::printAddress(byte *sensor_id){
  Serial.print("Sensor Address ");
  for(byte i=0; i < 8; i++) {
    Serial.print("0x");
    if (sensor_id[i] < 16) {
      Serial.print('0');
    }
    Serial.print(sensor_id[i], HEX);
    if (i < 7) {
      Serial.print(", ");
    }
  }
  Serial.print("\r\n");
}
