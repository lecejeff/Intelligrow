//***************************************************************************//
// File      : general.h
//
// Includes  : xc.h
//
//Jean-Francois Bilodeau    MPLab X v5.00   09/10/2018 
//****************************************************************************//

#ifndef __GENERAL_H_
#define	__GENERAL_H_

//#define FOSC 64000000 
//#define FCY (FOSC/4)    // FCY of 16MHz
//#define _XTAL_FREQ FOSC

//#define PPS_PORTA 0
//#define PPS_PORTB 1
//#define PPS_PORTC 2

void ft8xx_display_refresh();

void rtc_refresh_ticker ();
void WiFi_LED_ticker ();
void inactivity_counter_ticker();
void menu_dynamic_tick();
void sensor_refresh_ticker();
void water_counter_ticker();
void light_counter_ticker();
void minute_counter_tick();
void clear_inactivity_counter (void);

#define TANK_ECHO_PIN 15

#define LED_OFF 1
#define LED_ON 0

#define WIFI_ACT_LED_DEFAULT_STATUS LED_OFF
#define WATER_EMPTY_LED_DEFAULT_STATUS LED_OFF

#define INACTIVITY_SECOND_COUNTER 60

//#include <xc.h>
typedef struct
{
    unsigned char counter;
    unsigned char state;
    unsigned char flag;
    unsigned char do_once;
}BUTTON_STRUCT;

typedef struct 
{
    unsigned char wifi_act_led_status;
    unsigned char water_empty_led_status;
    unsigned char lcd_refresh_flag;
    unsigned char touch_tag_value;
    unsigned long touch_track;
    unsigned char menu_to_display;
    unsigned char calibration_flag;
    unsigned char previous_menu;
    unsigned char scan_tag_flag;
    unsigned char water_level_flag;
    unsigned int water_level;
    unsigned long micros_start;
    unsigned long micros_stop;
    unsigned char menu_screensaver;
    unsigned char plant_watering_flag;
    unsigned char inactivity_display_flag;
    unsigned int inactivity_counter;

    unsigned int light_sensor_daily[72];
    unsigned int temp_sensor_daily[72];
    unsigned int moist_sensor_daily[72];
}INTELLIGROW_struct;


void ESP_init (void);
void BUTTON_debounce (void);
unsigned char BUTTON_get_state (void);
void hex_to_ascii (unsigned char ucByte, unsigned char *ucByteH, unsigned char *ucByteL);
int bcd_to_decimal(unsigned char x);
unsigned int dec2bcd(unsigned int num);
unsigned char BCD_to_hex (unsigned char data);

#endif	/* GENERAL_H */

