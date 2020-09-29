//****************************************************************************//
// File      : io_expander.c
//
// Functions :  void EXPANDER_init (void);
//              void EXPANDER_write_register (unsigned char reg, unsigned char value);
//              unsigned char EXPANDER_read_register (unsigned char reg);
//              void EXPANDER_write_bit (unsigned char bit_pos, unsigned char value);
//              void EXPANDER_write_port (unsigned char value);
//              unsigned char EXPANDER_get_output_single (unsigned char bit_pos);
//              unsigned char EXPANDER_get_output_all (void);
//              unsigned char EXPANDER_get_input_all (void);
//              unsigned char EXPANDER_get_input_single (unsigned char bit_pos);
//              unsigned char EXPANDER_new_data_in (void);
//
// Includes  : io_expander.h
//Jean-Francois Bilodeau    MPLab X v5.00    10/09/2018 
//****************************************************************************//
#include "io_expander.h"
#include <Wire.h>


//**********************void EXPANDER_init (void)*****************************//
//Description : Function initializes IO expander
//
//Function prototype : void EXPANDER_init (void)
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : EXPANDER_init();
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/09/2018  
//****************************************************************************//
void IO_expander::init (void)
{   
    union_expander_output.mcp23008_out = 0xFF;
    // Set default configuration to MCP23008 registers   
    write_register(IOCON, 0x30);
    write_register(IODIR, 0x00);
    write_register(INTCON, 0x00);
    write_port(0x85 | (WIFI_ACT_LED_DEFAULT_STATUS << 6) | (WATER_EMPTY_LED_DEFAULT_STATUS << 4));
    // GPIO7 = FT8XX_POWERDOWN, 0 = Display OFF,    1 = Display ON               
    // GPIO6 = WATER_EMPTY,     0 = LED ON,         1 = LED OFF
    // GPIO5 = TANK_TRIG,       0 = N/A,            1 = TRIGGER
    // GPIO4 = Wifi_ACT,        0 = LED ON,         1 = LED OFF
    // GPIO3 = N/A
    // GPIO2 = PUMP_CTRL,       0 = Pump ON,        1 = Pump OFF
    // GPIO1 = N/A
    // GPIO0 = LIGHT_CTRL,      0 = Light ON,       1 = Light OFF
}

//*****void EXPANDER_write_register (unsigned char reg, unsigned char value)*****//
//Description : Function writes value to specified register on MCP23017
//
//Function prototype : void EXPANDER_write_register (unsigned char reg, unsigned char value)
//
//Enter params       : unsigned char reg : register of MCP23017
//                     unsigned char value : value to write to register
//
//Exit params        : None
//
//Function call      : EXPANDER_write_register(GPIOA, 0x00);
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/09/2018  
//****************************************************************************//
void IO_expander::write_register (unsigned char reg, unsigned char value)
{
    Wire.beginTransmission(MCP23008_ADR>>1);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

//***void EXPANDER_write_single (unsigned char bit_pos, unsigned char value)**//
//Description : Function changes value of single bit on output port of MCP23017
//
//Function prototype : void EXPANDER_write_single (unsigned char bit_pos, unsigned char value)
//
//Enter params       : unsigned char bit_pos : position of bit to change
//                     unsigned char value : new value of specified bit position
//
//Exit params        : None
//
//Function call      : EXPANDER_write_single(MPC23017_adr, LPOUT1, 0);
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/09/2018  
//****************************************************************************//
void IO_expander::write_bit (unsigned char bit_pos, unsigned char value)
{
    if (value==1){union_expander_output.mcp23008_out |= (1 << bit_pos);}
    else {union_expander_output.mcp23008_out &= ~(1 << bit_pos);}
    write_register(GPIO, union_expander_output.mcp23008_out);
}

//******void EXPANDER_write_all (unsigned char adr, unsigned char value)******//
//Description : Function changes value of whole output port on MCP23017
//
//Function prototype : void EXPANDER_write_all (unsigned char adr, unsigned char value)
//
//Enter params       : unsigned char adr : i2c adr of io expander
//                     unsigned char value : new value of port
//
//Exit params        : None
//
//Function call      : EXPANDER_write_all(MPC23017_adr, 0xF0);
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/09/2018  
//****************************************************************************//
void IO_expander::write_port (unsigned char value)
{
    union_expander_output.mcp23008_out = value;
    write_register(GPIO, union_expander_output.mcp23008_out);  
}

//******unsigned char EXPANDER_get_output_single (unsigned char bit_pos)******//
//Description : Function returns value of specified bit on output port
//
//Function prototype : unsigned char EXPANDER_get_output_single (unsigned char bit_pos)
//
//Enter params       : unsigned char bit_pos : position of bit to extract
//
//Exit params        : unsigned char : value of output bit
//
//Function call      : EXPANDER_get_output_single(EXPANDER_LPOUT1);
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/09/2018  
//****************************************************************************//
unsigned char IO_expander::get_output_single (unsigned char bit_pos)
{
    return ((union_expander_output.mcp23008_out>>bit_pos) & 0x01);
}

//****************unsigned char EXPANDER_get_output_all (void)****************//
//Description : Function returns value of all outputs
//
//Function prototype : unsigned char EXPANDER_get_output_all (void)
//
//Enter params       : None
//
//Exit params        : unsigned char : value of output port
//
//Function call      : EXPANDER_get_output_all();
//
//Jean-Francois Bilodeau    MPLab X v5.00    09/09/2018  
//****************************************************************************//
unsigned char IO_expander::get_output_all (void)
{
    return union_expander_output.mcp23008_out;
}
