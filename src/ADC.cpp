//***************************************************************************//
// File      : ADC.c
//
// Functions :  void ADC_init (void);
//              void ADC_start (void);
//              void ADC_stop (void);
//              unsigned char ADC_sample_status (void);
//              unsigned int ADC_get_channel (unsigned char channel);
//
// Includes  :  ADC.h
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
#include "ADC.h"
#include <Arduino.h>
#include <Wire.h>
//#include <Ticker.h>

// Git test 2

//Ticker adc_get_sample_ticker;
//Ticker adc_start_conversion_ticker;
I2C_ADC ADC_i2c;
             
//*************************void ADC_init (void)*******************************//
//Description : Function initializes the dsPIC ADC core
//
//Function prototype : void ADC_init (void)
//
//Enter params       : None
//
//Exit params        : None
//
//Function call      : ADC_init();
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
void I2C_ADC::init (void)
{             
    ADC_struct.adc_lsb_weight = 0.002;                                              // Based on FSR of ADS1015
    ADC_struct.adc_max_value = (unsigned int)((3.3 / ADC_struct.adc_lsb_weight)+1); // Max digital value that can be read by the ADC with signals referenced to 3V3 
    ADC_struct.temp_coeff = 0.01;                                                   // Volts / degreeC
    ADC_struct.temp_offset = 0.5;                                                   // Vout at Ta = 0 degreeC
    ADC_struct.res_ratio = 0.1808318;      

    ADC_struct.light_input = 0;
    ADC_struct.temperature_input = 0;
    ADC_struct.moisture_input = 0;
    ADC_struct.battery_monitor_input = 0;
    ADC_struct.temp_real = 0;
    ADC_struct.all_channel_sampled = 0;

    ADC_struct.data_ready = ADC_SAMPLE_NOT_READY;      //

    ADC_struct.auto_sample_counter = 0; 
    ADC_struct.auto_read_counter = 0;    

    //adc_get_sample_ticker.attach(0.05, std::bind(&I2C_ADC::get_sample, this));
    //adc_start_conversion_ticker.attach(0.05, std::bind(&I2C_ADC::start_conversion, this));
}

//***************unsigned char ADC_sample_status (void)***********************//
//Description : Function returns 1 when a new ADC sample is ready to be read
//
//Function prototype : unsigned char ADC_sample_status (void)
//
//Enter params       : None
//
//Exit params        : unsigned char : sample status (1 = ready)
//
//Function call      : unsigned char = ADC_sample_status();
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
unsigned char I2C_ADC::sample_status (void)
{
    if ((ADC_struct.data_ready == ADC_SAMPLE_READY) && (ADC_struct.all_channel_sampled == 1))
    {
        ADC_struct.data_ready = ADC_SAMPLE_NOT_READY;   // Clear flag
        return ADC_SAMPLE_READY;                        // Sample ready to be read
    }
    else return ADC_SAMPLE_NOT_READY;
}

//**********unsigned int ADC_get_channel (unsigned char channel)**************//
//Description : Function returns the value of the selected ADC channel
//
//Function prototype : unsigned int ADC_get_channel (unsigned char channel)
//
//Enter params       : unsigned char channel : ADC channel to read (see ADC.h)
//
//Exit params        : unsigned int : sample value between 0 and 2^12 - 1
//
//Function call      : unsigned int = ADC_get_channel(ADC_MOTOR_1_CHANNEL);
//
//Jean-Francois Bilodeau    MPLab X v5.10    11/02/2020 
//****************************************************************************//
unsigned int I2C_ADC::get_channel (unsigned char channel)
{
    switch(channel)
    {
        case ADC_LIGHT_INPUT:
            return ADC_struct.light_input;
            break;
            
        case ADC_MOISTURE_INPUT:
            return ADC_struct.moisture_input;
            break;
            
        case ADC_TEMPERATURE_INPUT:
            return ADC_struct.temperature_input;
            break;  

        case ADC_BATTERY_MONITOR_INPUT:
            return ADC_struct.battery_monitor_input;
            break; 

        default:
            return 0;
            break;        
    }
}

void I2C_ADC::start_conversion (unsigned char channel)
{
    unsigned int temp = 0;
    switch (channel)
    {
        // Light input on CH0
        case ADC_LIGHT_INPUT:
            Wire.beginTransmission(ADC_ADDRESS>>1);
            Wire.write(ADC_CONFIG_REGISTER);
            temp = (ADC_START_CONVERSION | ADC_CH0 | ADC_DATARATE_3300SPS | ADC_FSR_4V096 | ADC_SINGLE_SHOT | ADC_NO_ALRT);
            Wire.write((temp & 0xFF00)>>8);
            Wire.write(temp);    
            Wire.endTransmission();        
        break;

        // Moisture input on CH1
        case ADC_MOISTURE_INPUT:
            Wire.beginTransmission(ADC_ADDRESS>>1);
            Wire.write(ADC_CONFIG_REGISTER);
            temp = (ADC_START_CONVERSION | ADC_CH1 | ADC_DATARATE_3300SPS | ADC_FSR_4V096 | ADC_SINGLE_SHOT | ADC_NO_ALRT);
            Wire.write((temp & 0xFF00)>>8);
            Wire.write(temp);
            Wire.endTransmission();          
        break;

        // Tempetarure input on CH2
        case ADC_TEMPERATURE_INPUT:
            Wire.beginTransmission(ADC_ADDRESS>>1);
            Wire.write(ADC_CONFIG_REGISTER);
            temp = (ADC_START_CONVERSION | ADC_CH2 | ADC_DATARATE_3300SPS | ADC_FSR_4V096 | ADC_SINGLE_SHOT | ADC_NO_ALRT);
            Wire.write((temp & 0xFF00)>>8);
            Wire.write(temp);
            Wire.endTransmission();            
        break;

        // Battery monitor input on CH3
        case ADC_BATTERY_MONITOR_INPUT:
            Wire.beginTransmission(ADC_ADDRESS>>1);
            Wire.write(ADC_CONFIG_REGISTER);
            temp = (ADC_START_CONVERSION | ADC_CH3 | ADC_DATARATE_3300SPS | ADC_FSR_4V096 | ADC_SINGLE_SHOT | ADC_NO_ALRT);
            Wire.write((temp & 0xFF00)>>8);
            Wire.write(temp);
            Wire.endTransmission();           
        break;           
    }
}

unsigned int I2C_ADC::start_then_read_conversion (unsigned char channel)
{
    switch (channel)
    {
        // Light input on CH0
        case ADC_LIGHT_INPUT:
            start_conversion(channel);      
            delayMicroseconds(350);  // (1 / datarate) + power-up  wait time before conversion completed    
            get_sample(channel);
            return get_channel(channel); 
        break;

        // Moisture input on CH1
        case ADC_MOISTURE_INPUT:
            start_conversion(channel);          
            delayMicroseconds(350);  // (1 / datarate) + power-up  wait time before conversion completed        
            get_sample(channel);
            return get_channel(channel); 
        break;

        // Tempetarure input on CH2
        case ADC_TEMPERATURE_INPUT:
            start_conversion(channel);           
            delayMicroseconds(350);  // (1 / datarate) + power-up  wait time before conversion completed
            get_sample(channel);
            return get_channel(channel);          
        break;

        // Battery monitor input on CH3
        case ADC_BATTERY_MONITOR_INPUT:
            start_conversion(channel);        
            delayMicroseconds(350);  // (1 / datarate) + power-up  wait time before conversion completed      
            get_sample(channel);
            return get_channel(channel);      
        break;

        default:
            return 0;
        break;            
    }
}

void I2C_ADC::get_sample(unsigned char channel)
{  
    switch (channel)
    {
        case ADC_LIGHT_INPUT:
            Wire.beginTransmission(ADC_ADDRESS>>1);
            Wire.write(ADC_CONVERSION_REGISTER);
            Wire.endTransmission();   

            Wire.requestFrom(ADC_ADDRESS>>1, 2);
            ADC_struct.conv_result[0] = Wire.read();
            ADC_struct.conv_result[1] = Wire.read();
            ADC_struct.light_input = (((ADC_struct.conv_result[0] << 8) | ADC_struct.conv_result[1])>>4);
        break;  

        case ADC_MOISTURE_INPUT:
            Wire.beginTransmission(ADC_ADDRESS>>1);
            Wire.write(ADC_CONVERSION_REGISTER);
            Wire.endTransmission();   

            Wire.requestFrom(ADC_ADDRESS>>1, 2);
            ADC_struct.conv_result[0] = Wire.read();
            ADC_struct.conv_result[1] = Wire.read();
            ADC_struct.moisture_input = (((ADC_struct.conv_result[0] << 8) | ADC_struct.conv_result[1])>>4);
        break; 

        case ADC_TEMPERATURE_INPUT:
            Wire.beginTransmission(ADC_ADDRESS>>1);
            Wire.write(ADC_CONVERSION_REGISTER);
            Wire.endTransmission();   

            Wire.requestFrom(ADC_ADDRESS>>1, 2);
            ADC_struct.conv_result[0] = Wire.read();
            ADC_struct.conv_result[1] = Wire.read();
            ADC_struct.temperature_input = (((ADC_struct.conv_result[0] << 8) | ADC_struct.conv_result[1])>>4);
            ADC_struct.temp_real = (long)(((double)(ADC_struct.temperature_input * ADC_struct.adc_lsb_weight)  - ADC_struct.temp_offset) / ADC_struct.temp_coeff);
        break;      

        case ADC_BATTERY_MONITOR_INPUT:
            Wire.beginTransmission(ADC_ADDRESS>>1);
            Wire.write(ADC_CONVERSION_REGISTER);
            Wire.endTransmission();   

            Wire.requestFrom(ADC_ADDRESS>>1, 2);
            ADC_struct.conv_result[0] = Wire.read();
            ADC_struct.conv_result[1] = Wire.read();
            ADC_struct.battery_monitor_input = (((ADC_struct.conv_result[0] << 8) | ADC_struct.conv_result[1])>>4);  
            ADC_struct.bat_real = (long)((ADC_struct.battery_monitor_input*ADC_struct.adc_lsb_weight)/ADC_struct.res_ratio);     
        break;  
    }                         
}