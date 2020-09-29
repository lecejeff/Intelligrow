//***************************************************************************//
// File      : ADC.h
//
// Functions :  void ADC_init (void);
//              void ADC_start (void);
//              void ADC_stop (void);
//              unsigned char ADC_sample_status (void);
//              unsigned int ADC_get_channel (unsigned char channel);
//
// Includes  :  general.h
//           
// Purpose   :  Driver for the dsPIC33EP ADC core
//              ADC input on 4 seperate channels :
//              1) Motor 1 current sense
//              2) Motor 2 current sense
//              3) Intellitrol analog input control
//              4) Battery monitor
//
//Jean-Francois Bilodeau    MPLab X v5.10    10/02/2020  
//****************************************************************************//
#ifndef __ADC_H_
#define	__ADC_H_

#include "general.h"

#define ADC_ADDRESS                 0x92

// ADC CONFIG REGISTER 
#define ADC_START_CONVERSION        0x8000
#define ADC_CONVERSION_REGISTER     0x00
#define ADC_CONFIG_REGISTER         0x01
#define ADC_LOW_THRES_REGISTER      0x02
#define ADC_HIGH_THRES_REGISTER     0x03

#define ADC_LIGHT_INPUT             0
#define ADC_MOISTURE_INPUT          1
#define ADC_TEMPERATURE_INPUT       2
#define ADC_BATTERY_MONITOR_INPUT   3

#define ADC_CH0                     0x4000
#define ADC_CH1                     0x5000
#define ADC_CH2                     0x6000
#define ADC_CH3                     0x7000

#define ADC_CONTINUOUS              0x0000
#define ADC_SINGLE_SHOT             0x0100

#define ADC_FSR_4V096               0x0200

#define ADC_DATARATE_128SPS         0x0000
#define ADC_DATARATE_250SPS         0x0020
#define ADC_DATARATE_490SPS         0x0040
#define ADC_DATARATE_920SPS         0x0060
#define ADC_DATARATE_1600SPS        0x0080
#define ADC_DATARATE_2400SPS        0x00A0
#define ADC_DATARATE_3300SPS        0x00C0

#define ADC_NO_ALRT 0x000F

#define ADC_AVERAGE_LENGTH 1024

#define ADC_SAMPLE_READY 1
#define ADC_SAMPLE_NOT_READY 0

#define ADC_CHANNEL_COUNT 4

typedef struct
{
    unsigned char conv_result[2];
    unsigned int light_input;
    unsigned int temperature_input;
    unsigned int moisture_input;
    unsigned int battery_monitor_input;
    long temp_real;
    long bat_real;
    unsigned char data_ready;
    unsigned char auto_sample_counter;
    unsigned char auto_read_counter;
    unsigned char all_channel_sampled;

    double adc_lsb_weight;
    unsigned int adc_max_value;
    double temp_coeff;
    double temp_offset;
    double res_ratio;
}STRUCT_ADC;

class I2C_ADC
{
    public:
        void init (void);
        void get_sample (unsigned char channel);
        void start_conversion (unsigned char channel);
        unsigned char sample_status (void);
        unsigned int get_channel (unsigned char channel);
        unsigned int start_then_read_conversion (unsigned char channel);
        STRUCT_ADC ADC_struct;

    private:
};

#endif	

