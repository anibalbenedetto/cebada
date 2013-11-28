#include "Arduino.h"
#include "EEPROM.h"
#include "Control.h"

Control::Control():
  data_first_address(10), 
  data_rows(20), 
  data_index_sp(0), 
  data_index_hi(1), 
  data_index_id(2), 
  data_index_mo(10)
{
  set_point = 0;
  histeresis = 1;
  for (int j=0; j < 8; j++) sensor_id[j] = 0;
  block_mode = 0;
  
  temp = 0.0;
  output_cold = 1;
  output_heat = 1;
  block_status = 0;
  
};

void Control::ReadBlock(unsigned char index)
{
	unsigned char data_position = (index * data_rows ) + data_first_address;
	
	set_point = EEPROM.read(data_position + data_index_sp);
	histeresis = EEPROM.read(data_position + data_index_hi);	
	for (int j=0; j < 8; j++) sensor_id[j] = EEPROM.read(data_position + data_index_id + j);  
	block_mode = EEPROM.read(data_position + data_index_mo);
	
        Serial.print("Sensor #"); Serial.println(index);
      for(byte i=0; i < 8; i++) {
        Serial.print("0x");
        if (sensor_id[i] < 16) {
          Serial.print('0');                        // add a leading '0' if required.
        }
        Serial.print(sensor_id[i], HEX);                 // print the actual value in HEX
        if (i < 7) {
          Serial.print(", ");
        }
      }
      Serial.print("\r\n");

}

void Control::WriteBlock(unsigned char index)
{
	unsigned char data_position = (index * data_rows ) + data_first_address;
	
        EEPROM.write(data_position + data_index_sp, set_point);
        EEPROM.write(data_position + data_index_hi, histeresis);
        
        for (int j=0; j < 8; j++) EEPROM.write(data_position + data_index_id + j, sensor_id[j]);
        EEPROM.write(data_position + data_index_mo, block_mode);
}

void Control::setSensorId(unsigned char addr[])
{
	for (int j=0; j < 8; j++) sensor_id[j] = addr[j]; 	
};

unsigned char* Control::getSensorId()
{
  return sensor_id;
};

float Control::getTemp(){
  return temp;
}

void Control::setTemp(float _temp){
  temp = _temp;
}

unsigned char Control::getStatus(){
  return block_status;
}

unsigned char Control::getSetPoint(){
  return set_point;
}

unsigned char Control::getHisteresis(){
  return histeresis;
}

unsigned char Control::getMode(){
  return block_mode;
}

void Control::setSetPoint(unsigned char newValue){
  set_point = newValue;
}

void Control::setHisteresis(unsigned char newValue){
  histeresis = newValue;
}

void Control::setMode(unsigned char newValue){
  block_mode = newValue;
}

void Control::setStatus(unsigned char newValue){
  block_status = newValue;
}

void Control::updateStatus(){
  
  if(block_status == CONTROL_BLOCK_STATUS_ERROR){
    return;
  }
  else{
    switch(block_mode){
        case CONTROL_BLOCK_MODE_DISABLE:
          block_status = CONTROL_BLOCK_STATUS_OFF;
          break;
        case CONTROL_BLOCK_MODE_COLD:
          if( ( (int)temp ) > set_point )
            block_status = CONTROL_BLOCK_STATUS_COLD_ON;
          else
            block_status = CONTROL_BLOCK_STATUS_OFF;
          break;
        case CONTROL_BLOCK_MODE_HEAT:
          if( ( (int)temp ) < set_point )
            block_status = CONTROL_BLOCK_STATUS_HEAT_ON;
          else
            block_status = CONTROL_BLOCK_STATUS_OFF;
          break;
        case CONTROL_BLOCK_MODE_COLD_HEAT:
          if( ( (int)temp ) > set_point )
            block_status = CONTROL_BLOCK_STATUS_COLD_ON;
          else if( ( (int)temp ) < set_point )
            block_status = CONTROL_BLOCK_STATUS_HEAT_ON;
          else
            block_status = CONTROL_BLOCK_STATUS_OFF;
          break;
    }
  }
}
