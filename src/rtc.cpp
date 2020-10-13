#include "rtc.h"
#include <Wire.h>

char *day_str_lut[7] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
char *month_str_lut[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};

void RTC_class::init (unsigned char mode)
{   
    if (mode == RTC_INITIAL_POWERUP)
    {      
        rtc_struct.hour = dec2bcd(8);
        rtc_struct.minute = dec2bcd(7);
        rtc_struct.second = dec2bcd(0);
        rtc_struct.day = dec2bcd(TUESDAY);
        rtc_struct.date = dec2bcd(13);
        rtc_struct.month = dec2bcd(OCTOBER);
        rtc_struct.year = 2020;
        rtc_struct.day_str = day_str_lut[rtc_struct.day - 1];
        rtc_struct.month_str = month_str_lut[rtc_struct.month - 1];      
                                                     // Wait for any previous transaction to be overt
        Wire.beginTransmission(RTC_I2C_ADDRESS>>1);
        Wire.write(RTC_SECOND_REGISTER);
        Wire.write(rtc_struct.second | 0x80);       //s 
        Wire.write(rtc_struct.minute);              //m
        Wire.write(rtc_struct.hour & 0x3F);         //h
        Wire.write(rtc_struct.day & 0xF7);          //day
        Wire.write(rtc_struct.date);                //date  
        Wire.write(rtc_struct.month);               //month
        Wire.write(rtc_struct.year);                //year
        Wire.endTransmission(); 
    }
}

void RTC_class::set_time (RTC_STRUCT new_time)
{
    rtc_struct.second = dec2bcd(new_time.second) | 0x80;
    rtc_struct.minute = dec2bcd(new_time.minute);
    rtc_struct.hour = dec2bcd(new_time.hour) & 0x3F;
    rtc_struct.day = dec2bcd(new_time.day) & 0xF7;
    rtc_struct.date = dec2bcd(new_time.date);
    rtc_struct.month = dec2bcd(new_time.month);
    rtc_struct.year = new_time.year;

    Wire.beginTransmission(RTC_I2C_ADDRESS>>1);
    Wire.write(RTC_SECOND_REGISTER);
    Wire.write(rtc_struct.second);
    Wire.write(rtc_struct.minute);
    Wire.write(rtc_struct.hour);
    Wire.write(rtc_struct.day);
    Wire.write(rtc_struct.date);
    Wire.write(rtc_struct.month);
    Wire.write(rtc_struct.year);
    Wire.endTransmission();                
}

void RTC_class::get_time (void)
{
    Wire.beginTransmission(RTC_I2C_ADDRESS>>1);
    Wire.write(RTC_SECOND_REGISTER);
    Wire.endTransmission();
    Wire.requestFrom(RTC_I2C_ADDRESS>>1, RTC_READ_TIME_LENGTH);

    rtc_struct.second = bcd_to_decimal(Wire.read() & 0x7F);
    rtc_struct.minute = bcd_to_decimal(Wire.read());
    rtc_struct.hour = bcd_to_decimal(Wire.read() & 0x3F);
    rtc_struct.day = bcd_to_decimal(Wire.read() & 0x07);
    rtc_struct.date = bcd_to_decimal(Wire.read());
    rtc_struct.month = bcd_to_decimal(Wire.read() & 0x1F);
    rtc_struct.year = bcd_to_decimal(Wire.read());

    rtc_struct.day_str = day_str_lut[rtc_struct.day - 1];
    rtc_struct.month_str = month_str_lut[rtc_struct.month - 1];
}

unsigned char RTC_class::get_time_value (unsigned char type)
{
    switch (type)
    {
        case RTC_SECOND_REGISTER:
            return rtc_struct.second;
            break;
            
        case RTC_MINUTE_REGISTER:
            return rtc_struct.minute;
            break;
            
        case RTC_HOUR_REGISTER:
            return rtc_struct.hour;
            break;
            
        case RTC_WEEKDAY_REGISTER:
            return rtc_struct.day;
            break;
            
        case RTC_DATE_REGISTER:
            return rtc_struct.date;
            break;
            
        case RTC_MONTH_REGISTER:
            return rtc_struct.month;
            break;
            
        case RTC_YEAR_REGISTER:
            return rtc_struct.year;
            break;
            
        default:
            return 0;
            break;
    }
}



