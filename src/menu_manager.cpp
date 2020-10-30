#include "menu_manager.h"
#include "plant_manager.h"
#include "rtc.h"
#include "FT8XX.h"
#include "ADC.h"

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <Arduino.h>

#ifdef SCREEN_ENABLE

extern char *day_str_lut[7];
extern char *month_str_lut[12];
//extern PLANT_STRUCT PLANT_struct[PLANT_QTY];
//extern GARDEN_STRUCT GARDEN_struct;
extern FT8XX_EVE ft8xx;
extern RTC_class rtc;
extern PLANT_MANAGER plant;
extern I2C_ADC adc_i2c;
extern INTELLIGROW_struct intelligrow;
MENU_STRUCT menu_struct;
RTC_STRUCT new_time;
extern ESP8266WiFiMulti wifiMulti;

void MENU_MANAGER::init(void)
{    
    unsigned int i = 0;

    // Initialize default primitives values

    // Shared primitives
    ft8xx.CMD_gradient(0, 0, 0, 0x00AA00, 480, 272, 0x5555FF);
    ft8xx.set_touch_tag(FT_PRIM_GRADIENT, 0, 1); 

    // Screensaver primitives definition                                                                 // Set touch tag 1 to gradient primitive
    ft8xx.CMD_text(0, 238, 25, 25, OPT_CENTERX | OPT_CENTERY, "Intelligrow - Autonomous garden");     // Title
    ft8xx.CMD_text(1, 97, 240, 25, OPT_CENTERX | OPT_CENTERY, rtc.rtc_struct.day_str);                // Year
    ft8xx.CMD_text(2, 340, 240, 25, OPT_CENTERX | OPT_CENTERY, rtc.rtc_struct.month_str);             // Month 
    ft8xx.CMD_number(0, 211, 240, 25, OPT_CENTERX | OPT_CENTERY, rtc.rtc_struct.date);              // Date
    ft8xx.CMD_number(1, 435, 240, 25, OPT_CENTERX | OPT_CENTERY, rtc.rtc_struct.year);              // year
    ft8xx.CMD_clock(0, 240, 136, 70, OPT_FLAT | OPT_NOBACK, rtc.rtc_struct.hour, rtc.rtc_struct.minute, rtc.rtc_struct.second, 0);   

    // Main menu primitives definition
    ft8xx.CMD_button(0, 7, 217, 147, 48, 22, OPT_FLAT, "Plant growth");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 0, 2);                                                                    // Set touch tag 2 to "Plant Parameters" button  
    ft8xx.CMD_button(1, 165, 217, 147, 48, 22, OPT_FLAT, "Garden parameters");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 1, 3);                                                                    // Set touch tag 3 to "Garden parameters" button
    ft8xx.CMD_button(2, 324, 217, 147, 48, 22, OPT_FLAT, "General settings");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 2, 4);                                                                    // Set touch tag 4 to "General settings" button

    // General settings menu
    ft8xx.CMD_button(3, 7, 217, 147, 48, 22, OPT_FLAT, "Return");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 3, 5);   
    ft8xx.CMD_button(4, 165, 217, 147, 48, 22, OPT_FLAT, "WiFi parameters");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 4, 6);   
    ft8xx.CMD_button(5, 324, 217, 147, 48, 22, OPT_FLAT, "Calibrate screen");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 5, 7);   

    ft8xx.CMD_text(3, 65, 60, 22, OPT_CENTER, "Incident light");
    ft8xx.CMD_scrollbar(0, 136, 55, 123, 12, OPT_FLAT, 120, 67, 1650);
    ft8xx.CMD_text(4, 65, 95, 22, OPT_CENTER, "Temperature");
    ft8xx.CMD_scrollbar(1, 136, 90, 123, 12, OPT_FLAT, 120, 5, 125);
    ft8xx.CMD_text(5, 65, 130, 22, OPT_CENTER, "Soil moisture");
    ft8xx.CMD_scrollbar(2, 136, 125, 123, 12, OPT_FLAT, 120, 67, 1650);
    ft8xx.CMD_text(6, 65, 165, 22, OPT_CENTER, "Water level");
    ft8xx.CMD_scrollbar(3, 136, 160, 123, 12, OPT_FLAT, 120, 5, 100);
    ft8xx.CMD_text(7, 65, 200, 22, OPT_CENTER, "Battery monitor");
    ft8xx.CMD_scrollbar(4, 136, 195, 123, 12, OPT_FLAT, 120, 67, 1650);

    ft8xx.CMD_text(8, 350, 60, 22, OPT_CENTER, "Date / Time");
    ft8xx.CMD_text(9, 350, 90, 22, OPT_CENTER, "Monday");
    ft8xx.set_touch_tag(FT_PRIM_TEXT, 9, 8);  
    ft8xx.CMD_text(10, 340, 120, 22, OPT_CENTER, "Date : ");
    ft8xx.CMD_number(2, 375, 120, 22, OPT_CENTER, 1);
    ft8xx.set_touch_tag(FT_PRIM_NUMBER, 2, 9);  
    ft8xx.CMD_text(11, 350, 150, 22, OPT_CENTER, "January");
    ft8xx.set_touch_tag(FT_PRIM_TEXT, 11, 10);  
    // ft8xx.CMD_text(12, 330, 180, 22, OPT_CENTER, "Year : ");
    // ft8xx.CMD_number(3, 375, 180, 22, OPT_CENTER, 2020);
    // ft8xx.set_touch_tag(FT_PRIM_NUMBER, 3, 11);  

    ft8xx.CMD_number(4, 328, 180, 22, OPT_CENTER, 7);
    ft8xx.set_touch_tag(FT_PRIM_NUMBER, 4, 12);  
    ft8xx.CMD_text(13, 342, 179, 22, OPT_CENTER, ":");
    ft8xx.CMD_number(5, 356, 180, 22, OPT_CENTER, 59);
    ft8xx.set_touch_tag(FT_PRIM_NUMBER, 5, 13);  
    ft8xx.CMD_text(14, 372, 179, 22, OPT_CENTER, ":");
    ft8xx.CMD_number(6, 387, 180, 22, OPT_CENTER, 50);
    ft8xx.set_touch_tag(FT_PRIM_NUMBER, 6, 14);  

    ft8xx.CMD_button(6, 400, 120, 62, 27, 22, OPT_FLAT, "Save");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 6, 15);

    // Plant growth menu
    ft8xx.CMD_text(15, 180, 238, 20, OPT_CENTER, "Light");
    ft8xx.CMD_toggle(0, 210, 235, 30, 20, OPT_FLAT, 0, "On\xFFOff");
    ft8xx.set_touch_tag(FT_PRIM_TOGGLE, 0, 17);
    ft8xx.CMD_text(16, 280, 238, 20, OPT_CENTER, "Moisture");
    ft8xx.CMD_toggle(1, 315, 235, 30, 20, OPT_FLAT, 0, "On\xFFOff");
    ft8xx.set_touch_tag(FT_PRIM_TOGGLE, 1, 18);
    ft8xx.CMD_text(17, 390, 238, 20, OPT_CENTER, "Temperature");
    ft8xx.CMD_toggle(2, 435, 235, 30, 20, OPT_FLAT, 0, "On\xFFOff");
    ft8xx.set_touch_tag(FT_PRIM_TOGGLE, 2, 19);

    // Garden parameters menu
    ft8xx.CMD_text(18, 96, 60, 22, OPT_CENTER, "Water tank - Adjust limits");
    ft8xx.CMD_gauge(0, 48, 110, 45, OPT_NOBACK, 5, 5, intelligrow.water_level, plant.GARDEN_struct.water_empty_level);
    ft8xx.CMD_button(8, 105, 105, 30, 30, 22, OPT_FLAT, "+");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 8, 20);
    ft8xx.CMD_button(9, 145, 105, 30, 30, 22, OPT_FLAT, "-");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 9, 21);
    ft8xx.CMD_toggle(3, 115, 80, 50, 22, OPT_FLAT, TOGGLE_OFF, "Min\xFFMax");
    ft8xx.set_touch_tag(FT_PRIM_TOGGLE, 3, 22);
    ft8xx.CMD_number(7, 135, 150, 22, OPT_CENTER, plant.GARDEN_struct.water_empty_level);
    ft8xx.CMD_text(19, 157, 150, 22, OPT_CENTER, "cm");
    ft8xx.CMD_text(20, 63, 150, 22, OPT_CENTER, "Empty level:");
    ft8xx.CMD_text(21, 87, 170, 22, OPT_CENTER, "Send alert if empty?");
    ft8xx.CMD_toggle(4, 65, 190, 40, 22, OPT_FLAT, TOGGLE_OFF, "Yes\xFFNo");
    ft8xx.set_touch_tag(FT_PRIM_TOGGLE, 4, 23);

    ft8xx.CMD_text(22, 330, 60, 22, OPT_CENTER, "Pump on-time");
    ft8xx.CMD_button(10, 229, 75, 30, 30, 22, OPT_FLAT, "+");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 10, 24);
    ft8xx.CMD_button(11, 272, 75, 30, 30, 22, OPT_FLAT, "-");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 11, 25);        
    ft8xx.CMD_text(23, 357, 91, 22, OPT_CENTER, "Pump on for:");
    ft8xx.CMD_number(8, 415, 91, 22, OPT_CENTER, plant.GARDEN_struct.pump_on_time);
    ft8xx.CMD_text(24, 442, 91, 22, OPT_CENTER, "sec");

    ft8xx.CMD_text(25, 330, 130, 22, OPT_CENTER, "Adjust plant growth setpoints");

    ft8xx.CMD_text(26, 270, 160, 22, OPT_CENTER, "Light");
    ft8xx.CMD_number(9, 415, 160, 22, OPT_CENTER, plant.PLANT_struct.sun_intensity_setp);
    ft8xx.CMD_button(12, 310, 145, 30, 30, 22, OPT_FLAT, "+");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 12, 26);
    ft8xx.CMD_button(13, 350, 145, 30, 30, 22, OPT_FLAT, "-");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 13, 27);       

    ft8xx.CMD_text(27, 260, 200, 22, OPT_CENTER, "Moisture");
    ft8xx.CMD_number(10, 415, 200, 22, OPT_CENTER, plant.PLANT_struct.soil_moisture_setp);
    ft8xx.CMD_button(14, 310, 185, 30, 30, 22, OPT_FLAT, "+");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 14, 28);
    ft8xx.CMD_button(15, 350, 185, 30, 30, 22, OPT_FLAT, "-");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 15, 29);   

    // Graphic primitives
    ft8xx.CMD_number(11, 13, 49, 20, OPT_CENTER, adc_i2c.ADC_struct.adc_max_value); // Light Y axis max
    ft8xx.CMD_number(12, 38, 49, 20, OPT_CENTER, adc_i2c.ADC_struct.adc_max_value); // Moist Y axis max
    ft8xx.CMD_number(13, 63, 49, 20, OPT_CENTER, 50);                               // Temp Y axis max

    ft8xx.CMD_number(14, 38, 49, 20, OPT_CENTER, plant.PLANT_struct.sun_intensity_setp); // Light y axis setpoint
    ft8xx.CMD_number(15, 63, 49, 20, OPT_CENTER, plant.PLANT_struct.soil_moisture_setp); // Moist Y axis setpoint

    // ADDED to GARDEN PARAMETERS menu
    ft8xx.CMD_text(28, 260, 240, 22, OPT_CENTER, "Enlightment");
    ft8xx.CMD_button(16, 310, 225, 30, 30, 22, OPT_FLAT, "+");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 16, 30);  
    ft8xx.CMD_button(17, 350, 225, 30, 30, 22, OPT_FLAT, "-");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 17, 31); 
    ft8xx.CMD_number(16, 406, 240, 22, OPT_CENTER, plant.PLANT_struct.sun_time_h);   
    ft8xx.CMD_text(29, 445, 240, 22, OPT_CENTER, "Hours"); 

    ft8xx.CMD_text(30, 455, 160, 22, OPT_CENTER, "Lux"); 
    ft8xx.CMD_text(31, 450, 200, 22, OPT_CENTER, "RH"); 

    // WIFI_PARAMETERS_MENU
    ft8xx.CMD_button(18, 7, 50, 147, 48, 22, OPT_FLAT, "Scan Wi-Fi");
    ft8xx.set_touch_tag(FT_PRIM_BUTTON, 18, 32); 
    ft8xx.CMD_text(32, 210, 60, 22, OPT_CENTER, "Wi-Fi name");
    ft8xx.CMD_text(33, 400, 60, 22, OPT_CENTER, "Wi-Fi strength");
    ft8xx.CMD_text(34, 170, 80, 22, 0, "Wi-Fi name #1");
    ft8xx.CMD_number(17, 385, 80, 22, 0, 420);
    ft8xx.CMD_text(35, 170, 110, 22, 0, "Wi-Fi name #2");
    ft8xx.CMD_number(18, 385, 110, 22, 0, 420);
    ft8xx.CMD_text(36, 170, 140, 22, 0, "Wi-Fi name #3");
    ft8xx.CMD_number(19, 385, 140, 22, 0, 420);
    ft8xx.CMD_text(37, 170, 170, 22, 0, "Wi-Fi name #4");
    ft8xx.CMD_number(20, 385, 170, 22, 0, 420);
    ft8xx.CMD_text(38, 170, 200, 22, 0, "Wi-Fi name #5");
    ft8xx.CMD_number(21, 385, 200, 22, 0, 420);
    ft8xx.CMD_text(39, 170, 230, 22, 0, "Wi-Fi name #6");
    ft8xx.CMD_number(22, 385, 230, 22, 0, 420);

    // Initialize menu counters based on FT800 primitives
    menu_struct.day_counter = 2;   
    menu_struct.date_counter = st_Number[2].num;
    menu_struct.month_counter = 1;
    menu_struct.year_counter = st_Number[3].num;
    menu_struct.h_counter = st_Number[4].num;
    menu_struct.m_counter = st_Number[5].num;
    menu_struct.s_counter = st_Number[6].num;

    new_time.day = menu_struct.day_counter;
    new_time.date = menu_struct.date_counter;
    new_time.month = menu_struct.month_counter;
    new_time.year = menu_struct.year_counter;
    new_time.hour = menu_struct.h_counter;
    new_time.minute = menu_struct.m_counter;
    new_time.second = menu_struct.s_counter;

    GRAPH_struct.graph_sensor_pointer = 0;
    GRAPH_struct.graph_update_flag = 1;
    for (; i < GRAPH_ACC_VALUE; i++)
    {
        GRAPH_struct.light_sensor_daily[i] = 0;   // Initialize array at zeroes
        GRAPH_struct.temp_sensor_daily[i] = 0;   // Initialize array at zeroes
        GRAPH_struct.moist_sensor_daily[i] = 0;   // Initialize array at zeroes
    }
}

void MENU_MANAGER::display (unsigned char menu)
{    
    unsigned int i = 0, j = 0;
    unsigned char touch_tag = 0;
    unsigned int y_val = 0, x_val = 0;
    touch_tag = ft8xx.get_touch_tag();  // Fetch the latest touch tag value

    // Before displaying the screensaver, update the primitives variables
    switch (menu)
    {
        ///////////////////////////////////////////////////////////////////////////////////////////
        case SCREENSAVER_MENU: 
            ft8xx.modify_clock_hms(&st_Clock[0], rtc.rtc_struct.hour, rtc.rtc_struct.minute, rtc.rtc_struct.second);
            ft8xx.modify_element_string(1, FT_PRIM_TEXT, rtc.rtc_struct.day_str);
            ft8xx.modify_element_string(2, FT_PRIM_TEXT, rtc.rtc_struct.month_str);
            ft8xx.modify_number(&st_Number[0], NUMBER_VAL, rtc.rtc_struct.date);
            
            // Draw a touchable gradient and main text "Intelligrow - Autonomous garden"
            //
            ft8xx.start_new_dl();					// Start a new display list, reset ring buffer and ring pointer

            ft8xx.write_dl_long(CLEAR(1, 1, 1));
            ft8xx.write_dl_long(TAG_MASK(1));    
            ft8xx.write_dl_long(TAG(st_Gradient[0].touch_tag));
            ft8xx.draw_gradient(&st_Gradient[0]);
            ft8xx.write_dl_long(COLOR_RGB(255, 255, 255));
            ft8xx.draw_text(&st_Text[0]);   
            ft8xx.draw_text(&st_Text[1]);
            ft8xx.draw_text(&st_Text[2]);
            ft8xx.draw_clock(&st_Clock[0]);
            ft8xx.draw_number(&st_Number[0]);
            // ft8xx.draw_number(&st_Number[1]);   
            ft8xx.update_screen_dl();         		// Update display list    yield();         
                 
        break;

        ///////////////////////////////////////////////////////////////////////////////////////////
        case MAIN_MENU:
            ft8xx.start_new_dl();					// Start a new display list, reset ring buffer and ring pointer

            ft8xx.write_dl_long(CLEAR(1, 1, 1));
            ft8xx.write_dl_long(TAG_MASK(1)); 
            ft8xx.draw_gradient(&st_Gradient[0]);
            ft8xx.write_dl_long(COLOR_RGB(255, 255, 255)); 
            ft8xx.draw_text(&st_Text[0]);   
            ft8xx.set_context_fcolor(0x005500);
            ft8xx.set_context_bcolor(0x000000); 

            // Plant growth button, assign with TAG-2
            ft8xx.write_dl_long(TAG(st_Button[0].touch_tag));
            ft8xx.draw_button(&st_Button[0]);

            // Garden parameters button, assign with TAG-3
            ft8xx.write_dl_long(TAG(st_Button[1].touch_tag));              
            ft8xx.draw_button(&st_Button[1]);

            // General settings button, assign with TAG-4
            ft8xx.write_dl_long(TAG(st_Button[2].touch_tag));
            ft8xx.draw_button(&st_Button[2]);

            ft8xx.update_screen_dl();         		// Update display list    yield();    
        break;

        ///////////////////////////////////////////////////////////////////////////////////////////
        case GARDEN_PARAMETERS_MENU:

            if (intelligrow.water_level >= plant.GARDEN_struct.water_empty_level)
            {
                ft8xx.modify_gauge(&st_Gauge[0], GAUGE_VAL, plant.GARDEN_struct.water_empty_level);
            }
            else {ft8xx.modify_gauge(&st_Gauge[0], GAUGE_VAL, intelligrow.water_level);}
            

            // Touch tag on the "+" button of water level       
            if (touch_tag == st_Button[8].touch_tag)
            {        
                if (st_Toggle[3].state == TOGGLE_OFF)   // Water empty level
                {
                    plant.GARDEN_struct.water_empty_level++;
                    ft8xx.modify_number(&st_Number[7], NUMBER_VAL, plant.GARDEN_struct.water_empty_level);   
                    ft8xx.modify_gauge(&st_Gauge[0], GAUGE_RANGE, plant.GARDEN_struct.water_empty_level);  
                }        
                else                                    // Water full level
                {
                    plant.GARDEN_struct.water_full_level++;
                    ft8xx.modify_number(&st_Number[7], NUMBER_VAL, plant.GARDEN_struct.water_full_level);       
                }               
            }

            // Touch tag on the "-" button of water level
            if (touch_tag == st_Button[9].touch_tag)
            {
                if (st_Toggle[3].state == TOGGLE_OFF)   // Water empty level
                {
                    if (--plant.GARDEN_struct.water_empty_level < 1){plant.GARDEN_struct.water_empty_level = 1;}
                    ft8xx.modify_number(&st_Number[7], NUMBER_VAL, plant.GARDEN_struct.water_empty_level);   
                    ft8xx.modify_gauge(&st_Gauge[0], GAUGE_RANGE, plant.GARDEN_struct.water_empty_level); 
                }        
                else                                    // Water full level
                {
                    if (--plant.GARDEN_struct.water_full_level < 1){plant.GARDEN_struct.water_full_level = 1;}
                    ft8xx.modify_number(&st_Number[7], NUMBER_VAL, plant.GARDEN_struct.water_full_level);                       
                }                                                     
            }

            // Touch tag on the "+" button of Pump on-time
            if (touch_tag == st_Button[10].touch_tag)
            {                
                plant.GARDEN_struct.pump_on_time++;
                ft8xx.modify_number(&st_Number[8], NUMBER_VAL, plant.GARDEN_struct.pump_on_time);
            }

            // Touch tag on the "-" button of Pump on-time
            if (touch_tag == st_Button[11].touch_tag)
            {                
                if (--plant.GARDEN_struct.pump_on_time < 1){plant.GARDEN_struct.pump_on_time = 1;}
                ft8xx.modify_number(&st_Number[8], NUMBER_VAL, plant.GARDEN_struct.pump_on_time);
            }

            // Touch tag on the Max / Min water level adjust toggle
            if (touch_tag == st_Toggle[3].touch_tag)
            {                
                if (st_Toggle[3].state == TOGGLE_OFF)
                {
                    st_Toggle[3].state = TOGGLE_ON;
                    ft8xx.modify_element_string(20, FT_PRIM_TEXT, "Full level:");
                    ft8xx.modify_number(&st_Number[7], NUMBER_VAL, plant.GARDEN_struct.water_full_level);
                }
                else
                {
                    st_Toggle[3].state = TOGGLE_OFF;
                    ft8xx.modify_element_string(20, FT_PRIM_TEXT, "Empty level:");
                    ft8xx.modify_number(&st_Number[7], NUMBER_VAL, plant.GARDEN_struct.water_empty_level);
                }
                
            }

            // Touch tag on the send alert if empty toggle
            if (touch_tag == st_Toggle[4].touch_tag)
            {                
                if (st_Toggle[4].state == TOGGLE_OFF)
                {
                    st_Toggle[4].state = TOGGLE_ON;
                }
                else
                {
                    st_Toggle[4].state = TOGGLE_OFF;
                }              
            }   

            // Touch tag on the Light + setpoint button
            if (touch_tag == st_Button[12].touch_tag)
            {   
                plant.PLANT_struct.sun_intensity_setp += 25;
                if (plant.PLANT_struct.sun_intensity_setp >= adc_i2c.ADC_struct.adc_max_value)
                {
                    plant.PLANT_struct.sun_intensity_setp = adc_i2c.ADC_struct.adc_max_value;
                    
                }
                ft8xx.modify_number(&st_Number[9], NUMBER_VAL, plant.PLANT_struct.sun_intensity_setp);
            }   

            // Touch tag on the Light - setpoint button
            if (touch_tag == st_Button[13].touch_tag)
            {   
                if (plant.PLANT_struct.sun_intensity_setp >= 25)
                {
                    plant.PLANT_struct.sun_intensity_setp -= 25;
                    
                }
                ft8xx.modify_number(&st_Number[9], NUMBER_VAL, plant.PLANT_struct.sun_intensity_setp);
            }             


            // Touch tag on the moisture + setpoint button
            if (touch_tag == st_Button[14].touch_tag)
            {   
                plant.PLANT_struct.soil_moisture_setp += 25;
                if (plant.PLANT_struct.soil_moisture_setp >= adc_i2c.ADC_struct.adc_max_value)
                {
                    plant.PLANT_struct.soil_moisture_setp = adc_i2c.ADC_struct.adc_max_value;
                    
                }
                ft8xx.modify_number(&st_Number[10], NUMBER_VAL, plant.PLANT_struct.soil_moisture_setp);
            }   

            // Touch tag on the moisture - setpoint button
            if (touch_tag == st_Button[15].touch_tag)
            {   
                if (plant.PLANT_struct.soil_moisture_setp >= 25)
                {
                    plant.PLANT_struct.soil_moisture_setp -= 25;
                    
                }
                ft8xx.modify_number(&st_Number[10], NUMBER_VAL, plant.PLANT_struct.soil_moisture_setp);
            } 

            // Touch tag on the Enlightment + button
            if (touch_tag == st_Button[16].touch_tag)
            {
                if (++plant.PLANT_struct.sun_time_h > 16)
                {
                    plant.PLANT_struct.sun_time_h = 16;
                }
                ft8xx.modify_number(&st_Number[16], NUMBER_VAL, plant.PLANT_struct.sun_time_h);
            }

            // Touch tag on the Enlightment - button
            if (touch_tag == st_Button[17].touch_tag)
            {
                if (--plant.PLANT_struct.sun_time_h < 8)
                {
                    plant.PLANT_struct.sun_time_h = 8;
                }
                ft8xx.modify_number(&st_Number[16], NUMBER_VAL, plant.PLANT_struct.sun_time_h);
            }            

            ft8xx.start_new_dl();					// Start a new display list, reset ring buffer and ring pointer

            ft8xx.write_dl_long(CLEAR(1, 1, 1));
            ft8xx.write_dl_long(TAG_MASK(1)); 
            ft8xx.draw_gradient(&st_Gradient[0]);
            ft8xx.write_dl_long(COLOR_RGB(255, 255, 255)); 
            ft8xx.draw_text(&st_Text[0]);   
            ft8xx.set_context_fcolor(0x005500);
            ft8xx.set_context_bcolor(0x000000); 

            // 
            ft8xx.draw_text(&st_Text[18]);                      // "Water tank - Adjust limits"
            ft8xx.draw_gauge(&st_Gauge[0]);                     // Water tank gauge
            ft8xx.write_dl_long(TAG(st_Button[8].touch_tag));   // "+"
            ft8xx.draw_button(&st_Button[8]);
            ft8xx.write_dl_long(TAG(st_Button[9].touch_tag));   // "-"
            ft8xx.draw_button(&st_Button[9]);      
            ft8xx.write_dl_long(TAG(st_Toggle[3].touch_tag));   // Max / Min water level adjust     
            ft8xx.draw_toggle(&st_Toggle[3]);                   // Max / Min water level adjust
            ft8xx.draw_number(&st_Number[7]);
            ft8xx.draw_text(&st_Text[19]);                      // cm
            ft8xx.draw_text(&st_Text[20]);                      // Max water level
            ft8xx.draw_text(&st_Text[21]);                      // Send alert if empty?
            ft8xx.write_dl_long(TAG(st_Toggle[4].touch_tag));   // Yes / No alert toggle  
            ft8xx.draw_toggle(&st_Toggle[4]);                   // Yes / No alert toggle  

            ft8xx.draw_text(&st_Text[22]);                      // Pump on-time
            ft8xx.write_dl_long(TAG(st_Button[10].touch_tag));  // "+"
            ft8xx.draw_button(&st_Button[10]);           
            ft8xx.write_dl_long(TAG(st_Button[11].touch_tag));  // "-"
            ft8xx.draw_button(&st_Button[11]);   
            ft8xx.draw_text(&st_Text[23]);                      // Pump on for:
            ft8xx.draw_number(&st_Number[8]);  
            ft8xx.draw_text(&st_Text[24]);                      // sec        

            ft8xx.draw_text(&st_Text[25]);                      // Adjust plant growth setpoints

            ft8xx.draw_text(&st_Text[26]);                      // Light
            ft8xx.write_dl_long(TAG(st_Button[12].touch_tag));  // "+"
            ft8xx.draw_button(&st_Button[12]);
            ft8xx.write_dl_long(TAG(st_Button[13].touch_tag));  // "+"
            ft8xx.draw_button(&st_Button[13]);
            ft8xx.draw_number(&st_Number[9]);                   // 
            ft8xx.draw_text(&st_Text[30]);                      // Lux  


            ft8xx.draw_text(&st_Text[27]);                      // Moisture
            ft8xx.write_dl_long(TAG(st_Button[14].touch_tag));  // "+"
            ft8xx.draw_button(&st_Button[14]);
            ft8xx.write_dl_long(TAG(st_Button[15].touch_tag));  // "+"
            ft8xx.draw_button(&st_Button[15]);            
            ft8xx.draw_number(&st_Number[10]);                  //
            ft8xx.draw_text(&st_Text[31]);                      // %RH    


            ft8xx.draw_text(&st_Text[28]);                      // Enlightment
            ft8xx.write_dl_long(TAG(st_Button[16].touch_tag));  // "+"
            ft8xx.draw_button(&st_Button[16]);
            ft8xx.write_dl_long(TAG(st_Button[17].touch_tag));  // "+"
            ft8xx.draw_button(&st_Button[17]);            
            ft8xx.draw_number(&st_Number[16]);                  //      
            ft8xx.draw_text(&st_Text[29]);                      // Hours      

            
                    

            // Return button
            ft8xx.write_dl_long(TAG(st_Button[3].touch_tag));  
            ft8xx.draw_button(&st_Button[3]);

            ft8xx.update_screen_dl();         		// Update display list    yield();    
        break;

        ///////////////////////////////////////////////////////////////////////////////////////////
        case PLANT_GROWTH_MENU:
            // Update primitives based on touch tag 

            // Light touch tag
            if (touch_tag == st_Toggle[0].touch_tag)
            {
                if (st_Toggle[0].state == TOGGLE_OFF){st_Toggle[0].state = TOGGLE_ON;}
                else {st_Toggle[0].state = TOGGLE_OFF;}
            }
        
            // Moisture touch tag
            if (touch_tag == st_Toggle[1].touch_tag)
            {
                if (st_Toggle[1].state == TOGGLE_OFF){st_Toggle[1].state = TOGGLE_ON;}
                else {st_Toggle[1].state = TOGGLE_OFF;}
            }

            // Temperature touch tag
            if (touch_tag == st_Toggle[2].touch_tag)
            {
                if (st_Toggle[2].state == TOGGLE_OFF){st_Toggle[2].state = TOGGLE_ON;}
                else {st_Toggle[2].state = TOGGLE_OFF;}
            }                        

            ft8xx.start_new_dl();					// Start a new display list, reset ring buffer and ring pointer

            ft8xx.write_dl_long(CLEAR(1, 1, 1));
            ft8xx.write_dl_long(TAG_MASK(1)); 
            ft8xx.draw_gradient(&st_Gradient[0]);
            ft8xx.write_dl_long(COLOR_RGB(255, 255, 255)); 
            ft8xx.draw_text(&st_Text[0]);   
            ft8xx.set_context_fcolor(0x005500);
            ft8xx.set_context_bcolor(0x000000);         

            // Graphic setup
            // Graphic background
            
            GRAPH_struct.y_min = 45;
            GRAPH_struct.y_max = 210;
            GRAPH_struct.x_min = 0;
            GRAPH_struct.x_max = 480;

            // Graphic axis
            GRAPH_struct.axis_y_min = 55;
            GRAPH_struct.axis_y_max = 195;
            GRAPH_struct.axis_y_x_pos = 30;
            GRAPH_struct.axis_y_wanted_tick = GRAPH_Y_TICK;
            GRAPH_struct.axis_y_mark = ((GRAPH_struct.axis_y_max - GRAPH_struct.axis_y_min)/GRAPH_struct.axis_y_wanted_tick);

            GRAPH_struct.axis_x_min = 30;
            GRAPH_struct.axis_x_max = 462;
            GRAPH_struct.axis_x_y_pos = 195;
            GRAPH_struct.axis_x_wanted_tick = GRAPH_X_TICK;
            GRAPH_struct.axis_x_mark = ((GRAPH_struct.axis_x_max - GRAPH_struct.axis_x_min)/GRAPH_struct.axis_x_wanted_tick);

            // Display graph background
            ft8xx.write_dl_long(COLOR_RGB(0,0,0));                                      // Set graph background color
            ft8xx.write_dl_long(BEGIN(RECTS));
            ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.x_min,GRAPH_struct.y_min,0,0));
            ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.x_max,GRAPH_struct.y_max,0,0));
            ft8xx.write_dl_long(END());

            // Display graph axis
            ft8xx.write_dl_long(COLOR_RGB(255,255,255));                                // Set lines color
            ft8xx.write_dl_long(BEGIN(LINES));
            
            ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_y_x_pos, GRAPH_struct.axis_y_min, 0, 0));
            ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_y_x_pos, GRAPH_struct.axis_y_max, 0, 0));
            ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_x_min, GRAPH_struct.axis_x_y_pos, 0, 0));
            ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_x_max, GRAPH_struct.axis_x_y_pos, 0, 0));
            // Display x axis mark
            for (i=1;i<=GRAPH_struct.axis_x_wanted_tick; i++)
            {
                ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_y_x_pos + (i*GRAPH_struct.axis_x_mark), GRAPH_struct.axis_x_y_pos + 3, 0, 0));
                ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_y_x_pos + (i*GRAPH_struct.axis_x_mark), GRAPH_struct.axis_x_y_pos - 3, 0, 0));        
            }
            // Display y axis mark
            for (i=1;i<=GRAPH_struct.axis_y_wanted_tick; i++)
            {
                ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_y_x_pos - 3, GRAPH_struct.axis_x_y_pos - (i*GRAPH_struct.axis_y_mark), 0, 0));
                ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_y_x_pos + 3, GRAPH_struct.axis_x_y_pos - (i*GRAPH_struct.axis_y_mark), 0, 0));                 
            }  
            ft8xx.write_dl_long(END());  
            // Display origin '0'
            ft8xx.write_dl_long(BEGIN(BITMAPS));
            ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_x_min - 10, GRAPH_struct.axis_y_max, 20, '0'));
            ft8xx.write_dl_long(END());  
             
            ft8xx.write_dl_long(COLOR_RGB(164,164,164));                                // Set points color  
            ft8xx.write_dl_long(BEGIN(POINTS));
            ft8xx.write_dl_long(POINT_SIZE(16));   
            // Display points grid
            for (i=1;i<=GRAPH_struct.axis_x_wanted_tick; i++)
            {
                for (j=1;j<=GRAPH_struct.axis_y_wanted_tick; j++)
                {
                    ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_y_x_pos + (i*GRAPH_struct.axis_x_mark), GRAPH_struct.axis_x_y_pos - (j*GRAPH_struct.axis_y_mark), 0, 0));
                }                   
            } 
            ft8xx.write_dl_long(END()); 
            // Display sensor values and setpoints

            // Light sensor values
            ft8xx.write_dl_long(COLOR_RGB(255,170,0));                                  // Set points color  
            ft8xx.draw_text(&st_Text[15]);                                              // Light
            // Display the daily light data points
            ft8xx.write_dl_long(BEGIN(POINTS));
            ft8xx.write_dl_long(POINT_SIZE(48));   
            if (st_Toggle[0].state == TOGGLE_OFF)
            {
                for (i=0; i < GRAPH_ACC_VALUE; i++)
                {
                    y_val = y_map(GRAPH_struct.axis_y_min, GRAPH_struct.axis_y_max, 0, adc_i2c.ADC_struct.adc_max_value, GRAPH_struct.light_sensor_daily[i]);
                    x_val = x_map(GRAPH_struct.axis_x_min, GRAPH_struct.axis_x_max, GRAPH_struct.axis_x_mark, GRAPH_X_MINOR, i);
                    ft8xx.write_dl_long(VERTEX2II(x_val, y_val, 0, 0));
                }
                ft8xx.write_dl_long(END()); 
                ft8xx.write_dl_long(BEGIN(LINES));                                          // Display the setpoint line
                y_val = y_map(GRAPH_struct.axis_y_min, GRAPH_struct.axis_y_max, 0, adc_i2c.ADC_struct.adc_max_value, plant.PLANT_struct.sun_intensity_setp);
                ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_x_min, y_val, 0, 0));
                ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_x_max, y_val, 0, 0));            
                ft8xx.write_dl_long(END());
                ft8xx.modify_number(&st_Number[14], NUMBER_X, 13);
                ft8xx.modify_number(&st_Number[14], NUMBER_Y, y_val);
                ft8xx.modify_number(&st_Number[14], NUMBER_VAL, plant.PLANT_struct.sun_intensity_setp);
                ft8xx.draw_number(&st_Number[11]);           
                ft8xx.draw_number(&st_Number[14]);
            }
            

            // Moist sensor values
            ft8xx.write_dl_long(COLOR_RGB(0,0,255));                                // Set points color  
            ft8xx.draw_text(&st_Text[16]);                                          // Moisture
            // Display the daily moist data points                       
            ft8xx.write_dl_long(BEGIN(POINTS));  
            ft8xx.write_dl_long(POINT_SIZE(48));          
            if (st_Toggle[1].state == TOGGLE_OFF)
            {
                for (i=0; i < GRAPH_ACC_VALUE; i++)
                {
                    y_val = y_map(GRAPH_struct.axis_y_min, GRAPH_struct.axis_y_max, 0, adc_i2c.ADC_struct.adc_max_value, GRAPH_struct.moist_sensor_daily[i]);
                    x_val = x_map(GRAPH_struct.axis_x_min, GRAPH_struct.axis_x_max, GRAPH_struct.axis_x_mark, GRAPH_X_MINOR, i);
                    ft8xx.write_dl_long(VERTEX2II(x_val, y_val, 0, 0));               
                }
                ft8xx.write_dl_long(END()); 
                ft8xx.write_dl_long(BEGIN(LINES));                                      // Display the setpoint line    
                y_val = y_map(GRAPH_struct.axis_y_min, GRAPH_struct.axis_y_max, 0, adc_i2c.ADC_struct.adc_max_value, plant.PLANT_struct.soil_moisture_setp);
                ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_x_min, y_val, 0, 0));
                ft8xx.write_dl_long(VERTEX2II(GRAPH_struct.axis_x_max, y_val, 0, 0));
                ft8xx.write_dl_long(END());  
                ft8xx.modify_number(&st_Number[15], NUMBER_Y, y_val);
                ft8xx.modify_number(&st_Number[15], NUMBER_X, 13);
                ft8xx.modify_number(&st_Number[15], NUMBER_VAL, plant.PLANT_struct.soil_moisture_setp);
                ft8xx.draw_number(&st_Number[12]);           
                ft8xx.draw_number(&st_Number[15]);            
            }
            

            // // Temp sensor values
            ft8xx.write_dl_long(COLOR_RGB(255,0,0));                                // Set points color  
            ft8xx.draw_text(&st_Text[17]);                      // Temperature
            ft8xx.write_dl_long(BEGIN(POINTS)); 
            ft8xx.write_dl_long(POINT_SIZE(48));              
            if (st_Toggle[2].state == TOGGLE_OFF)
            {
                for (i=0; i < GRAPH_ACC_VALUE; i++)
                {
                    y_val = y_map(GRAPH_struct.axis_y_min, GRAPH_struct.axis_y_max, 0, 50, GRAPH_struct.temp_sensor_daily[i]);
                    x_val = x_map(GRAPH_struct.axis_x_min, GRAPH_struct.axis_x_max, GRAPH_struct.axis_x_mark, GRAPH_X_MINOR, i);
                    ft8xx.write_dl_long(VERTEX2II(x_val, y_val, 0, 0));
                }
                ft8xx.write_dl_long(END());
                ft8xx.draw_number(&st_Number[13]);                                 
            }
             

            ft8xx.write_dl_long(COLOR_RGB(255,255,255));                                //        
            // Text and toggle to enable graphic views  
            ft8xx.write_dl_long(TAG(st_Toggle[0].touch_tag));                 
            ft8xx.draw_toggle(&st_Toggle[0]); 
            ft8xx.write_dl_long(TAG(st_Toggle[1].touch_tag));      
            ft8xx.draw_toggle(&st_Toggle[1]);            
            ft8xx.write_dl_long(TAG(st_Toggle[2].touch_tag)); 
            ft8xx.draw_toggle(&st_Toggle[2]);                                          

            // Return button
            ft8xx.write_dl_long(TAG(st_Button[3].touch_tag));  
            ft8xx.draw_button(&st_Button[3]);

            ft8xx.update_screen_dl();         		// Update display list    yield();    
        break;

        ///////////////////////////////////////////////////////////////////////////////////////////
        case GENERAL_SETTINGS_MENU:
            // Update primitives based on touch tag 


            // Touch tag on the day of week, update the day of week variable
            if (touch_tag == st_Text[9].touch_tag)
            {                
                if (++menu_struct.day_counter > 6){menu_struct.day_counter = 0;}
                ft8xx.modify_element_string(9, FT_PRIM_TEXT, day_str_lut[menu_struct.day_counter]);
                new_time.day = menu_struct.day_counter + 1;
            }

            // Touch tag on the date, update the date variable
            if (touch_tag == st_Number[2].touch_tag)
            {
                if (++menu_struct.date_counter > 31){menu_struct.date_counter = 1;}
                ft8xx.modify_number(&st_Number[2], NUMBER_VAL, menu_struct.date_counter);
                new_time.date = menu_struct.date_counter;
            }

            // Touch tag on the month, update the month variable
            if (touch_tag == st_Text[11].touch_tag)
            {
                if (++menu_struct.month_counter > 12){menu_struct.month_counter = 1;}
                ft8xx.modify_element_string(11, FT_PRIM_TEXT, month_str_lut[menu_struct.month_counter-1]);
                new_time.month = menu_struct.month_counter;
            }

            // //Touch tag on the year, update the year variable
            // if (intelligrow.touch_tag_value == st_Number[3].touch_tag)
            // {
            //     ft8xx.clear_touch_tag();
            //     if (++menu_struct.year_counter > 2050){menu_struct.year_counter = 2020;}
            //     ft8xx.modify_number(&st_Number[3], menu_struct.year_counter);  
            //     new_time.year = menu_struct.year_counter - 2000;              
            // }

            //Touch tag on the hour, update the hour variable
            if (touch_tag == st_Number[4].touch_tag)
            {
                if (++menu_struct.h_counter > 23){menu_struct.h_counter = 0;}
                ft8xx.modify_number(&st_Number[4], NUMBER_VAL, menu_struct.h_counter);
                new_time.hour = menu_struct.h_counter;                  
            }

            //Touch tag on the minute, update the minute variable
            if (touch_tag == st_Number[5].touch_tag)
            {
                if (++menu_struct.m_counter > 59){menu_struct.m_counter = 0;}
                ft8xx.modify_number(&st_Number[5], NUMBER_VAL, menu_struct.m_counter);     
                new_time.minute = menu_struct.m_counter;                  
            }

            //Touch tag on the second, update the second variable
            if (touch_tag == st_Number[6].touch_tag)
            {
                if (++menu_struct.s_counter > 59){menu_struct.s_counter = 0;}
                ft8xx.modify_number(&st_Number[6], NUMBER_VAL, menu_struct.s_counter);  
                new_time.second = menu_struct.s_counter;                     
            }      

            // Touch tag on Save buton, save the time to the RTC
            if (touch_tag == st_Button[6].touch_tag)     
            {
                rtc.set_time(new_time);
            } 

            ft8xx.modify_scrollbar(&st_Scrollbar[0], SCROLLBAR_VAL, adc_i2c.ADC_struct.light_input);
            ft8xx.modify_scrollbar(&st_Scrollbar[1], SCROLLBAR_VAL, (unsigned int)adc_i2c.ADC_struct.temp_real);
            ft8xx.modify_scrollbar(&st_Scrollbar[2], SCROLLBAR_VAL, adc_i2c.ADC_struct.moisture_input);
            ft8xx.modify_scrollbar(&st_Scrollbar[3], SCROLLBAR_VAL, intelligrow.water_level);  
            ft8xx.modify_scrollbar(&st_Scrollbar[4], SCROLLBAR_VAL, adc_i2c.ADC_struct.battery_monitor_input);  


            ft8xx.start_new_dl();					// Start a new display list, reset ring buffer and ring pointer

            ft8xx.write_dl_long(CLEAR(1, 1, 1));
            ft8xx.write_dl_long(TAG_MASK(1)); 
            ft8xx.draw_gradient(&st_Gradient[0]);
            ft8xx.write_dl_long(COLOR_RGB(255, 255, 255)); 
            ft8xx.draw_text(&st_Text[0]);   
            ft8xx.set_context_fcolor(0x005500);
            ft8xx.set_context_bcolor(0x000000);         


            // Sensor scrollbars primitives
            ft8xx.draw_text(&st_Text[3]);
            ft8xx.draw_scrollbar(&st_Scrollbar[0]);
            ft8xx.draw_text(&st_Text[4]);
            ft8xx.draw_scrollbar(&st_Scrollbar[1]);
            ft8xx.draw_text(&st_Text[5]);
            ft8xx.draw_scrollbar(&st_Scrollbar[2]);
            ft8xx.draw_text(&st_Text[6]);
            ft8xx.draw_scrollbar(&st_Scrollbar[3]);       
            ft8xx.draw_text(&st_Text[7]);
            ft8xx.draw_scrollbar(&st_Scrollbar[4]);       

            // Date / time primitives
            ft8xx.draw_text(&st_Text[8]);                       // Date / Time
            ft8xx.write_dl_long(TAG(st_Text[9].touch_tag)); 
            ft8xx.draw_text(&st_Text[9]);                       // Day of week 
            ft8xx.draw_text(&st_Text[10]);                      // Date :                  
            ft8xx.write_dl_long(TAG(st_Number[2].touch_tag));   
            ft8xx.draw_number(&st_Number[2]);                   // date of month 
            ft8xx.write_dl_long(TAG(st_Text[11].touch_tag)); 
            ft8xx.draw_text(&st_Text[11]);                      // Month
            // ft8xx.draw_text(&st_Text[12]);                      // Year :
            // ft8xx.write_dl_long(TAG(st_Number[3].touch_tag));  
            // ft8xx.draw_number(&st_Number[3]);                   // Year nb     

            ft8xx.write_dl_long(TAG(st_Number[4].touch_tag));  
            ft8xx.draw_number(&st_Number[4]);
            ft8xx.draw_text(&st_Text[13]);
            ft8xx.write_dl_long(TAG(st_Number[5].touch_tag));  
            ft8xx.draw_number(&st_Number[5]);
            ft8xx.draw_text(&st_Text[14]);
            ft8xx.write_dl_long(TAG(st_Number[6].touch_tag));  
            ft8xx.draw_number(&st_Number[6]);

            // Save button
            ft8xx.write_dl_long(TAG(st_Button[6].touch_tag)); 
            ft8xx.draw_button(&st_Button[6]);

            // Return button
            ft8xx.write_dl_long(TAG(st_Button[3].touch_tag));  
            ft8xx.draw_button(&st_Button[3]);

            // Calibrate screen button
            if (intelligrow.calibration_flag == 0)
            {
                ft8xx.write_dl_long(TAG(st_Button[4].touch_tag));  
            }
            ft8xx.draw_button(&st_Button[4]);

            // WiFi parameters button
            ft8xx.write_dl_long(TAG(st_Button[5].touch_tag));  
            ft8xx.draw_button(&st_Button[5]);

            ft8xx.update_screen_dl();         		// Update display list    yield();                  
        break;  

            // Second level menu hierarchy
            case WIFI_PARAMETERS_MENU:

            //Touch tag on the "Scan Wi-FI button"
            if (touch_tag == st_Button[18].touch_tag)
            {
                Serial.print("Scan start ... ");
                int n = WiFi.scanNetworks();
                Serial.print(n);
                Serial.println(" network(s) found");
                for (int i = 0; i < 6; i++)
                {
                    String test;
                    char str[18];
                    Serial.print(WiFi.SSID(i));    
                    test = WiFi.SSID(i);
                    test.toCharArray(str, 18, 0);
                    ft8xx.modify_element_string(34 + i, FT_PRIM_TEXT, str);
                    Serial.print(" ");
                    Serial.println(WiFi.RSSI(i));
                    ft8xx.modify_number(&st_Number[17 + i], NUMBER_VAL, abs(WiFi.RSSI(i)));   
                }
                
                Serial.println();               
            }  

            ft8xx.start_new_dl();					// Start a new display list, reset ring buffer and ring pointer

            ft8xx.write_dl_long(CLEAR(1, 1, 1));
            ft8xx.write_dl_long(TAG_MASK(1)); 
            ft8xx.draw_gradient(&st_Gradient[0]);
            ft8xx.write_dl_long(COLOR_RGB(255, 255, 255)); 
            ft8xx.draw_text(&st_Text[0]);   
            ft8xx.set_context_fcolor(0x005500);
            ft8xx.set_context_bcolor(0x000000); 

            ft8xx.write_dl_long(COLOR_RGB(0, 0, 0));
            ft8xx.write_dl_long(BEGIN(RECTS));
            ft8xx.write_dl_long(VERTEX2II(163, 50, 0, 0));
            ft8xx.write_dl_long(VERTEX2II(470, 263, 0, 0));
            ft8xx.write_dl_long(END());
            ft8xx.write_dl_long(COLOR_RGB(255, 255, 255));

            ft8xx.draw_text(&st_Text[32]);                      // Wi-Fi name
            ft8xx.draw_text(&st_Text[33]);                      // WI-Fi strength

            ft8xx.draw_text(&st_Text[34]);                      // Wi-Fi name #1
            ft8xx.draw_number(&st_Number[17]);
            ft8xx.draw_text(&st_Text[35]);                      // Wi-Fi name #2
            ft8xx.draw_number(&st_Number[18]);
            ft8xx.draw_text(&st_Text[36]);                      // Wi-Fi name #3
            ft8xx.draw_number(&st_Number[19]);
            ft8xx.draw_text(&st_Text[37]);                      // Wi-Fi name #4
            ft8xx.draw_number(&st_Number[20]);
            ft8xx.draw_text(&st_Text[38]);                      // Wi-Fi name #5
            ft8xx.draw_number(&st_Number[21]);
            ft8xx.draw_text(&st_Text[39]);                      // Wi-Fi name #6   
            ft8xx.draw_number(&st_Number[22]);

            // Scan Wi-Fi button
            ft8xx.write_dl_long(TAG(st_Button[18].touch_tag));
            ft8xx.draw_button(&st_Button[18]);

            // Return button
            ft8xx.write_dl_long(TAG(st_Button[3].touch_tag));  
            ft8xx.draw_button(&st_Button[3]);

            ft8xx.update_screen_dl();         		// Update display list    yield();   
        break;         
    }
}

unsigned int MENU_MANAGER::y_map(unsigned int y_min, unsigned int y_max, unsigned int var_y_min, unsigned int var_y_max, unsigned int var)
{
    unsigned int variable_range = var_y_max - var_y_min;
    unsigned int graphic_range = y_max - y_min;
    unsigned char var_perc = (unsigned char)((var * 100) / variable_range);
    double div = (var_perc / 100.0);
    return (y_max - (graphic_range*div));
}

unsigned int MENU_MANAGER::x_map (unsigned int x_min, unsigned int x_max, unsigned int x_mark, unsigned int x_minor, unsigned int var)
{
    if (var <= GRAPH_ACC_VALUE)
    {
        return (x_min + ((var*x_mark)/x_minor));
    }
    else
    {
        return x_min;
    }
    
}

void MENU_MANAGER::graphic_handle(void)
{
    if (rtc.rtc_struct.second == 0)
    {
        if (rtc.rtc_struct.minute >= 0 && rtc.rtc_struct.minute < 20)
        {
            GRAPH_struct.graph_sensor_pointer = (rtc.rtc_struct.hour * 3)+0;
            if (rtc.rtc_struct.minute == 0)
            {
                GRAPH_struct.moist_sensor_daily[GRAPH_struct.graph_sensor_pointer] = adc_i2c.ADC_struct.moisture_input;
                GRAPH_struct.temp_sensor_daily[GRAPH_struct.graph_sensor_pointer] = adc_i2c.ADC_struct.temp_real;
                GRAPH_struct.light_sensor_daily[GRAPH_struct.graph_sensor_pointer] = adc_i2c.ADC_struct.light_input;
                GRAPH_struct.graph_update_flag = 1;
            }
        }

        if (rtc.rtc_struct.minute >= 20 && rtc.rtc_struct.minute < 40)
        {
            GRAPH_struct.graph_sensor_pointer = (rtc.rtc_struct.hour * 3)+1;  
            if (rtc.rtc_struct.minute == 20)
            {
                GRAPH_struct.moist_sensor_daily[GRAPH_struct.graph_sensor_pointer] = adc_i2c.ADC_struct.moisture_input;
                GRAPH_struct.temp_sensor_daily[GRAPH_struct.graph_sensor_pointer] = adc_i2c.ADC_struct.temp_real;
                GRAPH_struct.light_sensor_daily[GRAPH_struct.graph_sensor_pointer] = adc_i2c.ADC_struct.light_input;    
                GRAPH_struct.graph_update_flag = 1;
            }
        }

        if (rtc.rtc_struct.minute >= 40 && rtc.rtc_struct.minute < 60)
        {
            GRAPH_struct.graph_sensor_pointer = (rtc.rtc_struct.hour * 3)+2;
            if (rtc.rtc_struct.minute == 40)
            {
                GRAPH_struct.moist_sensor_daily[GRAPH_struct.graph_sensor_pointer] = adc_i2c.ADC_struct.moisture_input;
                GRAPH_struct.temp_sensor_daily[GRAPH_struct.graph_sensor_pointer] = adc_i2c.ADC_struct.temp_real;
                GRAPH_struct.light_sensor_daily[GRAPH_struct.graph_sensor_pointer] = adc_i2c.ADC_struct.light_input;       
                GRAPH_struct.graph_update_flag = 1;
            }
        } 
    }      
}
#endif