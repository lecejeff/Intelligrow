#ifndef __rtc_h
#define __rtc_h

#include "general.h"

#define RTC_I2C_ADDRESS 0xDE

#define RTC_BATTERY_WAKEUP 0
#define RTC_INITIAL_POWERUP 1

#define ALARM_QTY 2
#define ALARM_0 0
#define ALARM_1 1

#define RTC_SECOND_REGISTER 0x00
#define RTC_MINUTE_REGISTER 0x01
#define RTC_HOUR_REGISTER 0x02
#define RTC_WEEKDAY_REGISTER 0x03
#define RTC_DATE_REGISTER 0x04
#define RTC_MONTH_REGISTER 0x05
#define RTC_YEAR_REGISTER 0x06

#define RTC_CONTROL_REGISTER 0x07
#define RTC_OSCTRIM_REGISTER 0x08

#define RTC_ALM0_SECOND_REGISTER 0x0A
#define RTC_ALM0_MINUTE_REGISTER 0x0B
#define RTC_ALM0_HOUR_REGISTER 0x0C
#define RTC_ALM0_WEEKDAY_REGISTER 0x0D
#define RTC_ALM0_DATE_REGISTER 0x0E
#define RTC_ALM0_MONTH_REGISTER 0x0F

#define RTC_ALM1_SECOND_REGISTER 0x11
#define RTC_ALM1_MINUTE_REGISTER 0x12
#define RTC_ALM1_HOUR_REGISTER 0x13
#define RTC_ALM1_WEEKDAY_REGISTER 0x14
#define RTC_ALM1_DATE_REGISTER 0x15
#define RTC_ALM1_MONTH_REGISTER 0x16

#define RTC_SRAM_START_ADDRESS 0x20
#define RTC_SRAM_END_ADDRESS 0x5F

#define RTC_READ_TIME_LENGTH 7

#define MONDAY      1
#define TUESDAY     2
#define WEDNESDAY   3
#define THURSDAY    4
#define FRIDAY      5
#define SATURDAY    6
#define SUNDAY      7

#define JANUARY     1
#define FEBRUARY    2
#define MARCH       3
#define APRIL       4
#define MAY         5
#define JUNE        6
#define JULY        7
#define AUGUST      8
#define SEPTEMBER   9
#define OCTOBER     10
#define NOVEMBER    11
#define DECEMBER    12

typedef struct
{
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char date;
    unsigned char month;
    unsigned int year;
    unsigned char alarm_flag;
    unsigned char alarm_source;
    char *day_str;
    char *month_str;
}RTC_STRUCT;

class RTC_class
{
    public:
        void init (unsigned char mode);
        void set_time (RTC_STRUCT new_time);
        void get_time (void);
        void interrupt_handle (void);
        void enable_sqwout (void);
        unsigned char get_time_value (unsigned char type);
        RTC_STRUCT rtc_struct;
    private:
};
#endif