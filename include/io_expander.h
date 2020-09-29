//****************************************************************************//
// File      : io_expander.h
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
// Includes  : i2c.h
//Jean-Francois Bilodeau    MPLab X v5.00    10/09/2018 
//
//****************************************************************************//
#ifndef __io_expander_h__
#define __io_expander_h__

#include "general.h"

#define EXPANDER_OUT1 0
#define EXPANDER_OUT2 1
#define EXPANDER_OUT3 2
#define EXPANDER_OUT4 3
#define EXPANDER_OUT5 4
#define EXPANDER_OUT6 5
#define EXPANDER_OUT7 6
#define EXPANDER_OUT8 7

#define PUMP_ON 0
#define PUMP_OFF 1

#define LIGHT_ON 0
#define LIGHT_OFF 1

//typedef struct
//{
//    unsigned char hpin1 : 1;
//    unsigned char hpin2 : 1; 
//    unsigned char hpin3 : 1;
//    unsigned char hpin4 : 1;  
//    unsigned char lpin1 : 1;
//    unsigned char lpin2 : 1;  
//    unsigned char lpin3 : 1;
//    unsigned char lpin4 : 1;      
//}STRUCT_EXT_INPUT;
//
//typedef union
//{
//    STRUCT_EXT_INPUT struct_ext_in;
//    unsigned char mcp23017_in;
//    unsigned char FLAGA_data_in;
//    unsigned char FLAGB_data_in;
//}UNION_EXT_INPUT;

typedef struct
{
    unsigned char out1 : 1;
    unsigned char out2 : 1; 
    unsigned char out3 : 1;
    unsigned char out4 : 1;  
    unsigned char out5 : 1;
    unsigned char out6 : 1;  
    unsigned char out7 : 1;
    unsigned char out8 : 1;      
}STRUCT_EXT_OUTPUT;

typedef union
{
    STRUCT_EXT_OUTPUT struct_ext_out;
    unsigned char mcp23008_out;
}UNION_EXT_OUTPUT;

#define EXPANDER_nFT_PD_BIT         7
#define EXPANDER_nWATER_EMPTY_BIT   6
#define EXPANDER_TANK_TRIG_BIT      5
#define EXPANDER_nWIFI_ACT_BIT      4
#define EXPANDER_nPUMP_CTRL_BIT     2
#define EXPANDER_nLIGHT_CTRL_BIT    0

// Define I2C interface pins used in conjunction with I2C bus
#define MCP23008_ADR 0x40

//*****************************************************************************
// Definition for MCP23017 registers
#define IODIR      0x00    //Direction IO A Input(1) Output(0)
#define IPOL       0x01    //Polarity IO A Inverse if bit set 
#define GPINTEN    0x02    //Interrupt on change IO A active if set
#define DEFVAL     0x03    //Default compare value for IO A
#define INTCON     0x04    //set interrupt feature of specified pin IO A
#define IOCON      0x05    //IO A control register (Int, Mirror, Registers, etc)
#define GPPU       0x06    //Enable PullUP on specified IO A pin 
#define INTF       0x07    //IO A flag if interrupt occured on PIN
#define INTCAP     0x08    //Interrupt value saved in IO A pin
#define GPIO       0x09    //Write value to IO A port
#define IOLAT      0x0A    //Write value to LATCH A IO
//******************************************************************************

class IO_expander
{
    public:
        void init (void);
        void write_register (unsigned char reg, unsigned char value);
        void write_bit (unsigned char bit_pos, unsigned char value);
        void write_port (unsigned char value);
        unsigned char get_output_single (unsigned char bit_pos);
        unsigned char get_output_all (void);
        UNION_EXT_OUTPUT union_expander_output;

    private:
};

#endif