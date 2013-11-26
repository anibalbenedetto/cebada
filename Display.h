#ifndef Display_h
#define Display_h


#include "Arduino.h"
#include <LiquidCrystal_SR.h>

class Display
{
  private:
    int pin_LCD_ENABLE;
    int pin_LCD_CLOCK;
    int pin_LCD_DATA;
    
    byte num_blocks;
    byte current_block;
    float current_TEMP;
    byte current_SP;
    byte current_HI;
    byte current_MO;
    byte current_STATUS;
    
    byte current_param_value;
    byte current_system_mode;
    byte must_save;
    
    LiquidCrystal_SR lcd;
    
    byte charup[8];
    byte chardown[8];
    
     
  public:
     Display(int LCD_DATA, int LCD_CLOCK, int LCD_ENABLE);
     void begin();
     void print();
     
     void update_display();
     void update_system_mode(byte _current_system_mode);
     void update_block_values(byte _current_block, float _current_TEMP, byte _current_SP, byte _current_HI, byte _current_MO, byte _current_STATUS);
     void update_block_params(byte _current_block, byte _current_param_value, byte _current_STATUS);
     void printBlock();
     void print_UP_DOWN();
};

#endif
