#ifndef System_h
#define System_h

#include "Arduino.h"
#include "Control.h"
#include "Sensors.h"
#include "Display.h"

class System
{
  private:
    /***datos no volatiles***/
    unsigned char num_blocks; //número de bloques de control del sistema.
  
    /***datos volatiles***/
    Control *blocks[24]; //contiene todos los bloques en memoria.
    Sensors sensors;
    Display hd_display;
    int num_shift_registers; //número de registros de desplazamiento.
    unsigned char current_block; //bloque activo
    unsigned char system_mode;
    //unsigned char must_save;
    unsigned char current_option;
    unsigned char current_param_value;
    
    unsigned long last_display_loop_time;
    unsigned long last_temp_processing_time;
    
    int pin_OUT_ENABLE;
    int pin_OUT_CLOCK;
    int pin_OUT_DATA;
    int pin_IN_UP;
    int pin_IN_DOWN;
    int pin_IN_SET;
    int pin_IN_BACK;   
    
    void updateStatus();
    void updateOutputs();
    void updateTemps();
    
    void detectNewSensor();
    byte detectSensors(byte index);
    byte search_address_in_blocks(byte *sensor_id);
    
  public:
  
    enum Opt {UP, DOWN, SET, BACK};

    enum Mode {
      mode_oper,
      mode_oper_blk_id,
      mode_prog,
      mode_prog_blk,
      mode_prog_blk_id,
      mode_prog_blk_id_sp,
      mode_prog_blk_id_sp_edit,
      mode_prog_blk_id_hi,
      mode_prog_blk_id_hi_edit,
      mode_prog_blk_id_mo,
      mode_prog_blk_id_mo_edit,
      mode_prog_sen,
      mode_prog_sen_reset,
      mode_prog_sen_id,
      mode_prog_sys,
    };
    
    System(int ONE_WIRE_BUS, int OUT_ENABLE, int OUT_CLOCK, int OUT_DATA, int LCD_ENABLE, int LCD_CLOCK, int LCD_DATA, int IN_UP, int IN_DOWN, int IN_SET, int IN_BACK);
    void begin();    
    
    
    
    void setNumShiftRegisters(int _num_blocks);
    void Print();
    void input_handler(int opt);
    void read_inputs();
    void temp_processing();
};

#endif
