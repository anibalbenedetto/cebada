#include "Arduino.h"
#include "Display.h"
#include "System.h"

#include <LiquidCrystal_SR.h>

Display::Display(int LCD_DATA, int LCD_CLOCK, int LCD_ENABLE):
  lcd(LCD_DATA, LCD_CLOCK, LCD_ENABLE),
  num_blocks(0),
  current_block(0),
  current_param_value(0),
  current_system_mode(0),
  must_save(0)
{
  pin_LCD_ENABLE = LCD_ENABLE;
  pin_LCD_CLOCK = LCD_CLOCK;
  pin_LCD_DATA = LCD_DATA;
  
  
  charup[0] = B00100;
  charup[1] = B01110;
  charup[2] = B11111;
  charup[3] = B00100;
  charup[4] = B00100;
  charup[5] = B00000;
  charup[6] = B00000;
  charup[7] = B00000;
  
  chardown[0] = B00000;
  chardown[1] = B00000;
  chardown[2] = B00000;
  chardown[3] = B00100;
  chardown[4] = B00100;
  chardown[5] = B11111;
  chardown[6] = B01110;
  chardown[7] = B00100;
  
}

void Display::begin()
{
  Serial.begin(9600);  
  
  lcd.createChar(0, charup);
  lcd.createChar(1, chardown);
  lcd.begin(16,2); 
  lcd.home();
}

void Display::update_system_mode(byte _current_system_mode){
  current_system_mode = _current_system_mode;
}

void Display::update_block_values(byte _current_block, float _current_TEMP, byte _current_SP, byte _current_HI, byte _current_MO, byte _current_STATUS){
  current_block = _current_block;
  current_TEMP = _current_TEMP;
  current_SP = _current_SP;
  current_HI = _current_HI;
  current_MO = _current_MO;
  current_STATUS = _current_STATUS;
  
  /*
  Serial.print("current_block: "); Serial.println(current_block);
  Serial.print("current_TEMP: "); Serial.println(current_TEMP);
  Serial.print("current_SP: "); Serial.println(current_SP);
  Serial.print("current_HI: "); Serial.println(current_HI);
  Serial.print("current_MO: "); Serial.println(current_MO);
  Serial.print("current_STATUS: "); Serial.println(current_STATUS);
  */
}

void Display::update_block_params(byte _current_block, byte _current_param_value, byte _current_STATUS){
  current_block = _current_block;
  current_param_value = _current_param_value;
  current_STATUS = _current_STATUS;
}

void Display::update_display(){
  
  lcd.clear();
  
  switch(current_system_mode){
     case System::mode_oper:
       lcd.print("Modo:");
       lcd.setCursor(0,1);
       lcd.print(">Operativo");
       print_UP_DOWN();
       break;
     case System::mode_oper_blk_id:
       printBlock();
       break;
     case System::mode_prog:       
       lcd.print("Modo:");
       lcd.setCursor(0,1);
       lcd.print(">Programacion");
       print_UP_DOWN();       
       break;
     case System::mode_prog_blk:       
       lcd.print("Prog:");
       lcd.setCursor(0,1);
       lcd.print(">Bloque");
       print_UP_DOWN();    
       break;
     case System::mode_prog_blk_id:
       lcd.print("Prog/Blk:");
       lcd.setCursor(0,1);
       lcd.print(">Bloque #");
       lcd.print(current_block);
       print_UP_DOWN();
       
       break;
     case System::mode_prog_blk_id_sp:
     case System::mode_prog_blk_id_hi:
     case System::mode_prog_blk_id_mo:
     
       lcd.print("Prog/Blk/");
       lcd.print(current_block);
       lcd.print(":");
       lcd.setCursor(0,1);
       if(current_system_mode == System::mode_prog_blk_id_sp)
         lcd.print(">Set Point");
       if(current_system_mode == System::mode_prog_blk_id_hi)
         lcd.print(">Histeresis");
       if(current_system_mode == System::mode_prog_blk_id_mo)
         lcd.print(">Modo");
       
       print_UP_DOWN();
    
       break;
     case System::mode_prog_blk_id_sp_edit:
     case System::mode_prog_blk_id_hi_edit:
     case System::mode_prog_blk_id_mo_edit:
       
       lcd.print("Prog/Blk/");
       lcd.print(current_block);
       lcd.print(":");
       lcd.setCursor(0,1);
       if(current_STATUS == CONTROL_BLOCK_STATUS_MUST_SAVE)
         lcd.print(">Save ");
       else
         lcd.print(">Edit ");
       
       if(current_system_mode == System::mode_prog_blk_id_sp_edit)
         lcd.print("SP");
       if(current_system_mode == System::mode_prog_blk_id_hi_edit)
         lcd.print("HI");
       if(current_system_mode == System::mode_prog_blk_id_mo_edit)
         lcd.print("MO");
       
       lcd.print(": ");
       lcd.print(current_param_value);
       print_UP_DOWN();
       
       break;
     case System::mode_prog_sen:       
       lcd.print("Prog:");
       lcd.setCursor(0,1);
       lcd.print(">Sensores");
       print_UP_DOWN();     
       break;
     case System::mode_prog_sen_reset:
       lcd.print("Prog/Sen");
       lcd.setCursor(0,1);
       lcd.print("Descon sensores?");
       break;
     case System::mode_prog_sen_id:
       lcd.print("Prog/Sen/");
       lcd.print(current_block);
       lcd.print(":");
       lcd.setCursor(0,1);
       lcd.print("Conecte #");
       lcd.print(current_block);
       break;
     case System::mode_prog_sys:
       lcd.print("Prog:");
       lcd.setCursor(0,1);
       lcd.print(">Sistema");
       print_UP_DOWN();
       break;
  }
}


void Display::printBlock(){
  
  lcd.clear();
  lcd.print("#");
  lcd.print(current_block);
  lcd.setCursor(4,0);
  lcd.print("MO:");
  lcd.print(current_MO);  
  lcd.print(" ST:");
  lcd.print(current_STATUS);  
  lcd.setCursor(0,1);
  lcd.print("T:");
  lcd.print(current_TEMP);
  
  lcd.print(" SP:");
  lcd.print(current_SP);  
  
  print_UP_DOWN();  
}

void Display::print_UP_DOWN(){
  lcd.setCursor(15,0);
  lcd.write((uint8_t)0);
  lcd.setCursor(15,1);
  lcd.write((uint8_t)1);
}
