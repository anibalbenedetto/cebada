/*
Control: esta clase representa a una unidad de control.
Cada unidad de control estara asociada a un fermentador, banco de frio, banco de calor, camara, etc.
Las unidades de control poseen distintos modos de operacion que permiten configurar como se utilizara el hardware

*/

#ifndef Control_h
#define Control_h

#include "Arduino.h"

#define CONTROL_BLOCK_MODE_DISABLE     0 //el bloque está inactivo
#define CONTROL_BLOCK_MODE_COLD        1 //operando en modo frío
#define CONTROL_BLOCK_MODE_HEAT        2 //operando en modo calor
#define CONTROL_BLOCK_MODE_COLD_HEAT   3 //operando en modo frío calor automáticamente

#define CONTROL_BLOCK_STATUS_STARTUP    0 //el bloque debe iniciarse (leer temperatura, actualizar salidas)
#define CONTROL_BLOCK_STATUS_COLD_ON    1 //salida de frio activa
#define CONTROL_BLOCK_STATUS_HEAT_ON    2 //salida de calor activa
#define CONTROL_BLOCK_STATUS_OFF        3 //no se necesita corregir temperatura
#define CONTROL_BLOCK_STATUS_ERROR     	4 //no se puede leer la temperatura del sensor
#define CONTROL_BLOCK_STATUS_MUST_SAVE	5 //no se puede leer la temperatura del sensor


class Control
{
  private:
   
    /***datos no volatiles***/
    unsigned char set_point; //temperatura consigna
    unsigned char histeresis; //grados de recuperacion
    unsigned char sensor_id[8]; //id del ds18b20 grabado en la ROM del mismo
    unsigned char block_mode; //modo de operacion del bloque
    
    /***datos volatiles***/
    float temp; //última temperatura leída desde el sensor
    unsigned char output_cold; //posición del rele para control de frio
    unsigned char output_heat; //posición del rele para control de calor
    unsigned char block_status; // estado del modulo de control
      
    const unsigned char data_first_address;	                //ubicacion en los registros
    const unsigned char data_rows;				//registro reservados por bloque			
    const unsigned char data_index_sp;				//orden en tabla del set point
    const unsigned char data_index_hi;				//orden en tabla de la histeresis
    const unsigned char data_index_id;				//orden en tabla del id de sensor
    const unsigned char data_index_mo;				//orden en tabla del modo de operacion
    
  public:
  	Control();
        unsigned char getStatus();
        void updateStatus();
        float getTemp();
        void setTemp(float _temp);
        unsigned char getSetPoint();
        unsigned char getHisteresis();
        unsigned char getMode();      
        void setSetPoint(unsigned char newValue);
        void setHisteresis(unsigned char newValue);
        void setMode(unsigned char newValue);
        void setStatus(unsigned char newValue);
  	void ReadBlock(unsigned char index);
        void WriteBlock(unsigned char index);
        void setSensorId(unsigned char addr[]);
        unsigned char* getSensorId();
};

#endif
