#include <Arduino.h>
#include <EEPROM.h>
#include "Control.h"
#include "Display.h"
#include "System.h"


System::System(int ONE_WIRE_BUS, int OUT_ENABLE, int OUT_CLOCK, int OUT_DATA, int LCD_ENABLE, int LCD_CLOCK, int LCD_DATA, int IN_UP, int IN_DOWN, int IN_SET, int IN_BACK):
  sensors(ONE_WIRE_BUS),
  hd_display(LCD_DATA, LCD_CLOCK, LCD_ENABLE)
{
  pin_OUT_ENABLE = OUT_ENABLE;
  pin_OUT_CLOCK = OUT_CLOCK;
  pin_OUT_DATA = OUT_DATA;
  
  pin_IN_UP = IN_UP;
  pin_IN_DOWN = IN_DOWN;
  pin_IN_SET = IN_SET;
  pin_IN_BACK = IN_BACK;
  
  num_blocks = 0;
  current_block = 0;
  current_option = 0;
  num_shift_registers = 0;
  system_mode = 2;
  
  last_display_loop_time = millis();
  last_temp_processing_time = millis();
  
  for(int i = 0; i < 24; i++)
  {
    System::blocks[i] = new Control();
  }
  
  num_blocks = EEPROM.read(0);
};

void System::begin()
{
    pinMode(pin_OUT_ENABLE, OUTPUT);
    pinMode(pin_OUT_DATA, OUTPUT);
    pinMode(pin_OUT_CLOCK, OUTPUT); 
 
    pinMode(pin_IN_UP, INPUT);
    pinMode(pin_IN_DOWN, INPUT);
    pinMode(pin_IN_SET, INPUT);
    pinMode(pin_IN_BACK, INPUT);   
    
    hd_display.begin();
    sensors.begin();    
    
    Serial.print("num_blocks:"); Serial.println(num_blocks);
    Serial.print("num_sensors:"); Serial.println(sensors.getTotalDevices());
    
    if(num_blocks==255 || (num_blocks != sensors.getTotalDevices()) ){
      num_blocks = 0;
      num_shift_registers = 0;      
      system_mode = mode_prog;
    }
    else{ 
      
      /*
       * Se terenminan la cantidad de registros de desplazamiento (SR) necesarios.
       * Cada SR puede conmutar 8 salidas, por lo tanto 4 bloques de control
       * ya que cada uno utiliza 2 (una para frio y otra para calor)
       */
      setNumShiftRegisters(num_blocks);
      
      for (int i=0; i < num_blocks; i++){
        blocks[i]->ReadBlock(i);
      }
      
      system_mode = mode_oper;
      
    }
    hd_display.update_system_mode(system_mode);
    hd_display.update_display();
    Serial.print("system_mode:"); Serial.println(system_mode);

};

void System::updateStatus(){
  
  for (int i=0; i < num_blocks; i++){
    blocks[i]->updateStatus();
  }
}

void System::updateTemps(){
  
  sensors.requestTemperatures();
  for (byte i=0; i < num_blocks; i++){
    blocks[i]->setTemp( sensors.getTemperature( blocks[i]->getSensorId() ));
  }  
}

void System::updateOutputs(){
  Serial.print("num_shift_registers: "); Serial.println(num_shift_registers);
  
  byte _data[num_shift_registers]; //datos a escribir
  for (int j=0; j < num_shift_registers; j++) _data[j] = 0;
  
  for (int current_sr=0; current_sr < num_shift_registers; current_sr++){
    
    for (int i=0; ( (i < 4) && ( (current_sr * 4) + i) <= num_blocks); i++){
      
      int _current_block = (current_sr * 4) + i;
      int _rele_cold = (i*2);
      int _rele_heat = (i*2) + 1;
      
      byte _statusControl = (byte)blocks[_current_block]->getStatus();
      
      if( _statusControl == CONTROL_BLOCK_STATUS_COLD_ON ){
        bitWrite(_data[current_sr], _rele_cold, HIGH);
      }
      else if( _statusControl == CONTROL_BLOCK_STATUS_HEAT_ON ){
        bitWrite(_data[current_sr], _rele_heat, HIGH);
      }
      
    }
  }
  
  digitalWrite(pin_OUT_ENABLE, LOW);
  for (int current_sr=0; current_sr < num_shift_registers; current_sr++){
    Serial.print("ShiftRegisters: "); Serial.println(_data[current_sr]);
    shiftOut(pin_OUT_DATA, pin_OUT_CLOCK, MSBFIRST, _data[current_sr]);
  }
  digitalWrite(pin_OUT_ENABLE, HIGH);
  
}

byte System::detectSensors(byte index){
  byte located = 0;
  unsigned char sensors_id[8];
  

  sensors.reset_search();
  byte num_sensors = sensors.DiscoverOneWireDevices();
  //Serial.print("num_sensors:"); Serial.println(num_sensors);
  
  if(num_sensors == (index+1)){
  
    for(byte j=0; j<num_sensors; j++){
      sensors.getDeviceAddress(sensors_id, j);      
      located = search_address_in_blocks(sensors_id);
      if(!located)
        break;
    }
    
    if(!located){
      
      sensors.printAddress(sensors_id);
      
      num_blocks = index+1;
      EEPROM.write(0, num_blocks);
      setNumShiftRegisters(num_blocks);
      blocks[index]->setSensorId(sensors_id);
      blocks[index]->WriteBlock(index);
      return 0;
    }
    
  }
  
  return 1;  
}

byte System::search_address_in_blocks(byte *sensor_id){
  
  unsigned char *sensors_id_block;
  for (byte i=0; i < num_blocks; i++){
    
    sensors_id_block = blocks[i]->getSensorId();
    
    byte comp = 0;
    for(byte h=0; h<8; h++){
      if(sensors_id_block[h] == sensor_id[h]){
        comp++;
      }        
    }
    if(comp == 8){
      return 1;
    }
  }
  return 0;
}

void System::setNumShiftRegisters(int _num_blocks){  
  int _num_shift_registers = _num_blocks/4;
  num_shift_registers = _num_blocks/4;
  if(  ( _num_blocks % 4 ) >0 )
    num_shift_registers++;
}

void System::input_handler(int opt){
  
  switch(system_mode){
     case mode_oper:
       switch(opt){
         case UP:
         case DOWN:
           system_mode = mode_prog;
           break;
         case SET:
           system_mode = mode_oper_blk_id;
           break;
         case BACK:
           break;
       }
       break;
     case mode_oper_blk_id:
       switch(opt){
         case UP:
           current_block++;
           if(current_block == num_blocks)
            current_block = 0;
           break;
         case DOWN:
           if(current_block == 0)
             current_block = (num_blocks-1);
           else
             current_block--;
           break;
         case SET:
           break;
         case BACK:
           system_mode = mode_oper;
           break;
       }
       break;
     case mode_prog:
       switch(opt){
         case UP:
         case DOWN:
           system_mode = mode_oper;
           break;
         case SET:
           system_mode = mode_prog_blk;
           break;
         case BACK:
           break;
       }
       break;
     case mode_prog_blk:
       switch(opt){
         case UP:
           system_mode = mode_prog_sen;
           break;
         case DOWN:
           system_mode = mode_prog_sys;
           break;
         case SET:
           system_mode = mode_prog_blk_id;
           break;
         case BACK:
           system_mode = mode_prog;
           break;
       }
       break;
     case mode_prog_blk_id:
       switch(opt){
         case UP:
           current_block++;
           if(current_block == num_blocks)
            current_block = 0;
           break;
         case DOWN:
           if(current_block == 0)
             current_block = (num_blocks-1);
           else
             current_block--;
           break;
         case SET:
           system_mode = mode_prog_blk_id_sp;
           break;
         case BACK:
           system_mode = mode_prog_blk;
           break;
       }
       break;
     case mode_prog_blk_id_sp:
       switch(opt){
         case UP:
           system_mode = mode_prog_blk_id_hi;
           break;
         case DOWN:
           system_mode = mode_prog_blk_id_mo;
           break;
         case SET:
           system_mode = mode_prog_blk_id_sp_edit;
           current_param_value = blocks[current_block]->getSetPoint();
           break;
         case BACK:
           system_mode = mode_prog_blk_id;
           break;
       }    
       break;
     case mode_prog_blk_id_sp_edit:     
       switch(opt){
         case UP:
           current_param_value++;
           blocks[current_block]->setStatus(CONTROL_BLOCK_STATUS_MUST_SAVE);
           break;
         case DOWN:
           current_param_value--;
           blocks[current_block]->setStatus(CONTROL_BLOCK_STATUS_MUST_SAVE);
           break;
         case SET:
           blocks[current_block]->setSetPoint(current_param_value);
           blocks[current_block]->WriteBlock(current_block);
           blocks[current_block]->setStatus(CONTROL_BLOCK_STATUS_STARTUP);
           break;
         case BACK:
           system_mode = mode_prog_blk_id_sp;
           break; 
       }    
       break;
     case mode_prog_blk_id_hi:
       
       switch(opt){
         case UP:
           system_mode = mode_prog_blk_id_mo;
           break;
         case DOWN:
           system_mode = mode_prog_blk_id_sp;
           break;
         case SET:
           system_mode = mode_prog_blk_id_hi_edit;
           current_param_value = blocks[current_block]->getHisteresis();
           break;
         case BACK:
           system_mode = mode_prog_blk_id;
           break;
       } 
       break;
     case mode_prog_blk_id_hi_edit:
       
       switch(opt){
         case UP:
           current_param_value++;
           blocks[current_block]->setStatus(CONTROL_BLOCK_STATUS_MUST_SAVE);
           break;
         case DOWN:
           current_param_value--;
           blocks[current_block]->setStatus(CONTROL_BLOCK_STATUS_MUST_SAVE);
           break;
         case SET:
           blocks[current_block]->setHisteresis(current_param_value);
           blocks[current_block]->WriteBlock(current_block);
           blocks[current_block]->setStatus(CONTROL_BLOCK_STATUS_STARTUP);
           break;
         case BACK:
           system_mode = mode_prog_blk_id_hi;
           break;
       }
       break;
     case mode_prog_blk_id_mo:
       
       switch(opt){
         case UP:
           system_mode = mode_prog_blk_id_sp;
           break;
         case DOWN:
           system_mode = mode_prog_blk_id_hi;
           break;
         case SET:
           system_mode = mode_prog_blk_id_mo_edit;
           current_param_value = blocks[current_block]->getMode();
           break;
         case BACK:
           system_mode = mode_prog_blk;
           break;
       }
       break;
     case mode_prog_blk_id_mo_edit:
       
       switch(opt){
         case UP:
           current_param_value++;
           blocks[current_block]->setStatus(CONTROL_BLOCK_STATUS_MUST_SAVE);
           break;
         case DOWN:
           current_param_value--;
           blocks[current_block]->setStatus(CONTROL_BLOCK_STATUS_MUST_SAVE);
           break;
         case SET:
           blocks[current_block]->setMode(current_param_value);
           blocks[current_block]->WriteBlock(current_block);
           blocks[current_block]->setStatus(CONTROL_BLOCK_STATUS_STARTUP);           
           break;
         case BACK:
           system_mode = mode_prog_blk_id_mo;
           break;
       }
       break;
     case mode_prog_sen:
       switch(opt){
         case UP:
           system_mode = mode_prog_sys;
           break;
         case DOWN:
           system_mode = mode_prog_blk;
           break;
         case SET:
           system_mode = mode_prog_sen_reset;
           break;
         case BACK:
           system_mode = mode_prog;
           break;
       }
       break;
     case mode_prog_sen_reset:
       switch(opt){
         case UP:
         case DOWN:
           break;
         case SET:
           num_blocks = 0;
           EEPROM.write(0, num_blocks);
           current_block == 0;
           system_mode = mode_prog_sen_id;
           break;
         case BACK:
           system_mode = mode_prog_sen;
           break;
       }
       break;     
     case mode_prog_sen_id:
       switch(opt){
         case UP:
         case DOWN:
           break;
         case SET:
           if(detectSensors(current_block)){
             //realiza nuevamente.
             //no cambia de estado.
           }
           else{
             current_block++;
           }
           break;
         case BACK:
           system_mode = mode_prog_sen;
           break;
       }
       break;
     case mode_prog_sys:
       switch(opt){
         case UP:
           system_mode = mode_prog_blk;
           break;
         case DOWN:
           system_mode = mode_prog_sen;
           break;
         case SET:
           //todo
           break;
         case BACK:
           system_mode = mode_prog;
           break;
       }
       break;
  }
  
  hd_display.update_system_mode(system_mode);
  
  if(system_mode > mode_prog)
    hd_display.update_block_params(current_block, current_param_value, blocks[current_block]->getStatus());
  else{
    hd_display.update_block_values(
                  current_block,
                  blocks[current_block]->getTemp(),
                  blocks[current_block]->getSetPoint(),
                  blocks[current_block]->getHisteresis(),
                  blocks[current_block]->getMode(),
                  blocks[current_block]->getStatus()
                  );
     last_display_loop_time = millis();
  }
}

void System::read_inputs(){
  
  byte fire_event = 0;
  if(digitalRead(pin_IN_UP)==HIGH){
    input_handler(UP);
    fire_event++;
  }
  else if(digitalRead(pin_IN_DOWN)==HIGH){    
    input_handler(DOWN);
    fire_event++;
  }
  else if(digitalRead(pin_IN_SET)==HIGH){    
    input_handler(SET);
    fire_event++;
  }
  else if(digitalRead(pin_IN_BACK)==HIGH){    
    input_handler(BACK);
    fire_event++;
  }
  if(fire_event)
    hd_display.update_display();
  
  delay(300);
  
  /*
  if (Serial.available()) 
  {
    byte read = Serial.read();
    
    Serial.print("input..."); Serial.println(read);
    
    switch (read) 
    {
      case 'w': input_handler(UP); Serial.println("UP"); break;
      case 's': input_handler(DOWN); Serial.println("DOWN"); break;
      case 'd': input_handler(SET); Serial.println("SET"); break;
      case 'a': input_handler(BACK); Serial.println("BACK"); break;
    }
    hd_display.update_display();
  }
  */
  
}



void System::temp_processing(){
  
  if(system_mode != mode_oper_blk_id)
    return;
  
  updateTemps();
  updateStatus();
  updateOutputs();  
  
  if ( system_mode == mode_oper_blk_id )
  {
    if (millis() - last_display_loop_time >4000){
      
      current_block++;
      if(current_block >= num_blocks)
        current_block = 0;
      
            
      hd_display.update_block_values(
                  current_block,
                  blocks[current_block]->getTemp(),
                  blocks[current_block]->getSetPoint(),
                  blocks[current_block]->getHisteresis(),
                  blocks[current_block]->getMode(),
                  blocks[current_block]->getStatus()
                  );
      
      hd_display.update_display();
      last_display_loop_time = millis();
    }
  }
  
}
