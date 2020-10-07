//Test git

// Arduino framework includes
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Ticker.h>

// Intelligrow -oriented libraries
#include "general.h"
#include "rtc.h"
#include "plant_manager.h"
#include "menu_manager.h"
#include "io_expander.h"
#include "ADC.h"
#include "FT8XX.h"
#include "image_file.h"

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <FS.h>

// Wifi and web server variables
// const char* ssid     = "JFBBGF";        // Your WiFi ssid
// const char* password = "IngMed9496";    // Your Wifi password;
// const char* mdnsName = "intelligrow";        // Domain name for the mDNS responder

// ESP8266WebServer server(80);
// File fsUploadFile;                                    // a File variable to temporarily store the received file
// WiFiUDP UDP;                   // Create an instance of the WiFiUDP class to send and receive UDP messages
// IPAddress timeServerIP;        // The time.nist.gov NTP server's IP address
// const char* ntpServerName = "time.nist.gov";
// const int NTP_PACKET_SIZE = 48;          // NTP time stamp is in the first 48 bytes of the message
// byte packetBuffer[NTP_PACKET_SIZE];     // A buffer to hold incoming and outgoing packets
//


MENU_MANAGER menu;
I2C_ADC adc_i2c;
IO_expander expander_i2c;
RTC_class rtc;
PLANT_MANAGER plant;
FT8XX_EVE ft8xx;
INTELLIGROW_struct intelligrow;

unsigned char ByteH, ByteL;
unsigned char debug_uart_flag = 0;

Ticker ft8xx_update_ticker;
// Ticker btn_debounce_ticker;
Ticker time_update_ticker;
// Ticker Wifi_LED_refresh_ticker;
Ticker inactivity_ticker;
// Ticker menu_refresh_ticker;
Ticker sensor_update_ticker;
Ticker second_ticker;
Ticker water_ticker;

// Interrupts function should come before the setup() and loop()
ICACHE_RAM_ATTR void water_level_handle() 
{
    if (digitalRead(TANK_ECHO_PIN) == HIGH)
    {
        intelligrow.water_level_flag = 0;
        intelligrow.micros_start = micros();
    }
    
    else
    {      
        intelligrow.micros_stop = micros();
        intelligrow.water_level = ((intelligrow.micros_stop - intelligrow.micros_start) / 58); //(0.03434 / 2) = ~ 1 / 58
        intelligrow.micros_start = 0;
        intelligrow.micros_stop = 0;
        intelligrow.water_level_flag = 0;    
    }   
}

// void handleRoot() {
//   server.send(200, "text/plain", "Intelligrow v1_00-0a Web server is up and running");
// }

void setup() 
{
    ESP_init();

    Serial.begin(460800);

    Wire.pins(4, 5);
    Wire.begin();                                       // Initialize I2C bus on ESP8266
                                                        // Call Wire.piin() and Wire.begin() before FT8XX_init();
    expander_i2c.init();                                // Initialize IO expander
    SPI.begin();                                        // Initialize SPI bus on ESP8266
    SPI.beginTransaction(SPISettings(15000000, MSBFIRST, SPI_MODE0));

    ft8xx.init();                                       // Initialize LCD and touchpanel

    rtc.init(RTC_INITIAL_POWERUP);                      // Initialize real-time clock

    adc_i2c.init();                                     // Initialize I2C ADCs

    plant.init(PLANT_1);                                // Initialize default plants

    // WiFi.begin(ssid, password);             // Connect to the network
    // Serial.print("Attempting to connect to SSID: ");
    // Serial.print(ssid);

    // // Attempt to connect to WiFi network:
    // while (WiFi.status() != WL_CONNECTED) 
    // {
    //     Serial.print(".");
    //     // Connect to WPA/WPA2 network. Change this line if using open or WEP  network:
    //     // Wait 5 seconds for connection:
    //     delay(5000);
    // }

    // Serial.println("");
    // Serial.println("WiFi connected");
    // Serial.println("IP address: ");
    // Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP   

    // if(WiFi.status() == WL_CONNECTED) //If WiFi connected to hot spot then start mDNS
    // {
    //     if (!MDNS.begin("intelligrow")) 
    //     {             // Start the mDNS responder for esp8266.local
    //         Serial.println("Error setting up MDNS responder!");
    //     }
    //     else
    //     {
    //         Serial.println("mDNS responder started"); 
    //         server.on("/", handleRoot);  //Associate handler function to path
                
    //         server.begin();                           //Start server
    //         Serial.println("HTTP server started");          
    //     }
    // }

    menu.init();

    // Initialize application tickers
    // // These sould be the last function calls before going in the while(1)
    // Wifi_LED_refresh_ticker.attach(1, WiFi_LED_ticker);
    inactivity_ticker.attach(1, inactivity_counter_ticker);         // Inactivity counter service for automatic screensaver switch
    time_update_ticker.attach(0.25, rtc_refresh_ticker);            // Real-time clock update service
    sensor_update_ticker.attach(0.1, sensor_refresh_ticker);        // Sensors continuous refresh service
    ft8xx_update_ticker.attach_ms(16, ft8xx_display_refresh);       // FT8XX refresh service
    second_ticker.attach(1, light_counter_ticker);                  // 
    water_ticker.attach_ms(100, water_counter_ticker);              //   
}

void loop()
{
    // Flag set in the ft8xx_display_refresh() ticker service
    if (intelligrow.lcd_refresh_flag == 1)
    {
        // Update lcd_refresh flag and scan the actual TAG value.
        intelligrow.lcd_refresh_flag = 0;

        intelligrow.touch_tag_value = ft8xx.read_touch_tag();
        intelligrow.touch_track = ft8xx.rd32(REG_TRACKER);

        // Scan the touch tag value, and determine which object on the screen was touched
        if (intelligrow.touch_tag_value != 0)
        {
            clear_inactivity_counter(); 
            switch (intelligrow.touch_tag_value)
            {                   
                // Touch tag 1 on screensaver gradient
                // Touching the screensaver moves the UI to the main menu                
                case 1: 
                             
                    intelligrow.menu_to_display = MAIN_MENU;
                break;

                // Touch tag 2 on Plant growth button
                // Touching the "Plant growth" button moves the UI
                case 2:
                    intelligrow.previous_menu = MAIN_MENU;                    
                    intelligrow.menu_to_display = PLANT_GROWTH_MENU;
                break;

                // Touch tag 3 on Garden parameters button
                // Touching the "Garden parameters" button moves the UI
                case 3:
                    intelligrow.previous_menu = MAIN_MENU;
                    intelligrow.menu_to_display = GARDEN_PARAMETERS_MENU;
                break;

                // Touch tag 4 on General settings button
                // Touching the "General settings" button moves the UI
                case 4:
                    intelligrow.previous_menu = MAIN_MENU;
                    intelligrow.menu_to_display = GENERAL_SETTINGS_MENU;        
                break;

                // Touch tag 5 on Return button
                // Applicable to any menu that requires a return function
                case 5:
                    intelligrow.menu_to_display = intelligrow.previous_menu;
                break;

                // Touch tag 6 on WiFi parameters button
                //
                case 6:
                break;
                
                // Touch tag 7 runs a touchpanel calibration
                case 7: 
                    if (intelligrow.calibration_flag == 0)
                    {
                        intelligrow.calibration_flag = 1;
                        ft8xx.touchpanel_init();                    
                        intelligrow.previous_menu = MAIN_MENU;                    
                        intelligrow.menu_to_display = GENERAL_SETTINGS_MENU;      
                    }          
                break;

            }
        }
       
        // Show screensaver
        if (intelligrow.menu_screensaver == 1)
        {
            intelligrow.menu_to_display = SCREENSAVER_MENU;
        }

        menu.display(intelligrow.menu_to_display); 
        ft8xx.clear_touch_tag(); 
    }

    if (debug_uart_flag == 1)
    {
        debug_uart_flag = 0;
        Serial.write("Intelligrow Autonomous Garden - Debug"); 
        Serial.write(0x0D); Serial.write(0x0A);      
        Serial.write("Time : ");
        Serial.write(((dec2bcd(rtc.rtc_struct.hour) & 0xF0)>>4)+0x30); 
        Serial.write((dec2bcd(rtc.rtc_struct.hour) & 0x0F)+0x30);
        Serial.write(":");
        Serial.write(((dec2bcd(rtc.rtc_struct.minute) & 0xF0)>>4)+0x30); 
        Serial.write((dec2bcd(rtc.rtc_struct.minute) & 0x0F)+0x30);
        Serial.write(":");
        Serial.write(((dec2bcd(rtc.rtc_struct.second) & 0xF0)>>4)+0x30); 
        Serial.write((dec2bcd(rtc.rtc_struct.second) & 0x0F)+0x30); 
        Serial.write(0x0D); Serial.write(0x0A);          
        Serial.write("ADC0 (Light) : ");
        hex_to_ascii(((adc_i2c.ADC_struct.light_input & 0xFF00)>>8), &ByteH, &ByteL);
        Serial.write(ByteH); Serial.write(ByteL);
        hex_to_ascii(adc_i2c.ADC_struct.light_input, &ByteH, &ByteL);
        Serial.write(ByteH); Serial.write(ByteL);  
        Serial.write(0x0D); Serial.write(0x0A);      
        Serial.write("ADC1 (Moist): ");
        hex_to_ascii(((adc_i2c.ADC_struct.moisture_input & 0xFF00)>>8), &ByteH, &ByteL);
        Serial.write(ByteH); Serial.write(ByteL);    
        hex_to_ascii(adc_i2c.ADC_struct.moisture_input, &ByteH, &ByteL);
        Serial.write(ByteH); Serial.write(ByteL);  
        Serial.write(0x0D); Serial.write(0x0A);              
        Serial.write("ADC2 (Temp) : ");
        hex_to_ascii(((adc_i2c.ADC_struct.temperature_input & 0xFF00)>>8), &ByteH, &ByteL);
        Serial.write(ByteH); Serial.write(ByteL); 
        hex_to_ascii(adc_i2c.ADC_struct.temperature_input, &ByteH, &ByteL);
        Serial.write(ByteH); Serial.write(ByteL); 
        Serial.write(0x0D); Serial.write(0x0A);        
        Serial.write("ADC3 (BAT) : ");
        hex_to_ascii(((adc_i2c.ADC_struct.battery_monitor_input & 0xFF00)>>8), &ByteH, &ByteL);
        Serial.write(ByteH); Serial.write(ByteL); 
        hex_to_ascii(adc_i2c.ADC_struct.battery_monitor_input, &ByteH, &ByteL);
        Serial.write(ByteH); Serial.write(ByteL); 
        Serial.write(0x0D); Serial.write(0x0A);         
        Serial.write("Water level : ");
        Serial.write(((dec2bcd(intelligrow.water_level) & 0xF0)>>4)+0x30); 
        Serial.write((dec2bcd(intelligrow.water_level) & 0x0F)+0x30); 
        Serial.write(0x0D); Serial.write(0x0A);   
        Serial.write("Light counter : ");
        hex_to_ascii(((plant.PLANT_struct.sun_time_counter&0xFF000000)>>24), &ByteH, &ByteL); 
        Serial.write(ByteH); Serial.write(ByteL);  
        hex_to_ascii(((plant.PLANT_struct.sun_time_counter&0x00FF0000)>>16), &ByteH, &ByteL); 
        Serial.write(ByteH); Serial.write(ByteL);  
        hex_to_ascii(((plant.PLANT_struct.sun_time_counter&0x0000FF00)>>8), &ByteH, &ByteL); 
        Serial.write(ByteH); Serial.write(ByteL);  
        hex_to_ascii((plant.PLANT_struct.sun_time_counter&0x000000FF), &ByteH, &ByteL); 
        Serial.write(ByteH); Serial.write(ByteL);                          
        Serial.write(0x0D); Serial.write(0x0A);
        Serial.write("Light remainder : ");
        hex_to_ascii(((plant.PLANT_struct.sun_remainder&0xFF000000)>>24), &ByteH, &ByteL); 
        Serial.write(ByteH); Serial.write(ByteL);  
        hex_to_ascii(((plant.PLANT_struct.sun_remainder&0x00FF0000)>>16), &ByteH, &ByteL); 
        Serial.write(ByteH); Serial.write(ByteL);  
        hex_to_ascii(((plant.PLANT_struct.sun_remainder&0x0000FF00)>>8), &ByteH, &ByteL); 
        Serial.write(ByteH); Serial.write(ByteL);  
        hex_to_ascii((plant.PLANT_struct.sun_remainder&0x000000FF), &ByteH, &ByteL); 
        Serial.write(ByteH); Serial.write(ByteL);                     
        Serial.write(0x0D); Serial.write(0x0A); 
        Serial.write(0x0D); Serial.write(0x0A);
        Serial.write(0x0D); Serial.write(0x0A);
        Serial.write(0x0D); Serial.write(0x0A);
    }

    yield();
}

// Initialize ESP8266 peripherals and intelligrow variables
void ESP_init(void)
{
    intelligrow.water_empty_led_status = WATER_EMPTY_LED_DEFAULT_STATUS;
    intelligrow.wifi_act_led_status = WIFI_ACT_LED_DEFAULT_STATUS;
    intelligrow.touch_tag_value = 0;
    intelligrow.lcd_refresh_flag = 0;
    intelligrow.menu_to_display = SCREENSAVER_MENU;
    intelligrow.scan_tag_flag = 0;
    intelligrow.calibration_flag = 0;
    intelligrow.water_level_flag = 0;
    intelligrow.menu_screensaver = 0;
    intelligrow.plant_watering_flag = 0;
    intelligrow.inactivity_display_flag = 0;
    intelligrow.inactivity_counter = INACTIVITY_SECOND_COUNTER;

    pinMode(TANK_ECHO_PIN, INPUT);      //HC-SR04P ECHO pin is an input
    attachInterrupt(digitalPinToInterrupt(TANK_ECHO_PIN), water_level_handle, CHANGE);
}

// 60Hz display refresh service
// Update the actual display list only if a complete display list was previously built
void ft8xx_display_refresh()
{
    intelligrow.lcd_refresh_flag = 1;
}

// Periodically read the real-time clock and update the time registers
void rtc_refresh_ticker()
{
    //static unsigned char counter = 0;
    rtc.get_time();     
    menu.graphic_handle();                                   
}

void water_counter_ticker ()
{
    static unsigned int counter = 0;
    if ((rtc.rtc_struct.second == 0) && (rtc.rtc_struct.minute == plant.GARDEN_struct.water_time_m) && (rtc.rtc_struct.hour == plant.GARDEN_struct.water_time_h))
    {
        if (adc_i2c.ADC_struct.moisture_input >= plant.PLANT_struct.soil_moisture_setp)
        {
            if (intelligrow.plant_watering_flag == 0)
            {
                intelligrow.plant_watering_flag = 1;
            }                  
        }          
    }

    if (intelligrow.plant_watering_flag == 1)
    {
        if (++counter <= ((plant.GARDEN_struct.pump_on_time*1000)/100))
        {
            expander_i2c.write_bit(EXPANDER_nPUMP_CTRL_BIT, PUMP_ON);
        }
        else
        {
            expander_i2c.write_bit(EXPANDER_nPUMP_CTRL_BIT, PUMP_OFF);
            counter = 0;
            intelligrow.plant_watering_flag = 0;
        }
        
    }
}

// Called each second
void light_counter_ticker()
{
    static unsigned char remainder_calc_once = 0;
    static unsigned char do_once_1 = 0; 

    // Normal working hours are from 8hAM to 17H PM. Between this time, assumption to use the natural light of the sun.
    // If no light is present during this time, increment the illumination counters
    // The sensed light should be higher than the plant sun_intensity_setp variable
    if ((rtc.rtc_struct.hour >= 8) && (rtc.rtc_struct.hour < 16))
    {
        remainder_calc_once = 0;
        // If the amount of light sensed is higher than the illumination threshold, increment enlightment counter
        plant.PLANT_struct.sun_remainder = 1;
        if (adc_i2c.ADC_struct.light_input > plant.PLANT_struct.sun_intensity_setp)
        {
            plant.PLANT_struct.sun_time_counter++;     // second increment  
        }  
    }

    // Outside normal working hours
    // If a plant did not get enough illumination, here the remainder will decrement back to 0 while the lamp is activated
    else
    {
        if (remainder_calc_once == 0)
        {
            remainder_calc_once = 1;
            if ((plant.PLANT_struct.sun_time_counter / 3600) < plant.PLANT_struct.sun_time_h)
            {
                plant.PLANT_struct.sun_remainder = ((plant.PLANT_struct.sun_time_h * 3600) - plant.PLANT_struct.sun_time_counter);
                plant.PLANT_struct.sun_time_counter = 0;   // Reset illumination counter for the next day
            }
            else
            {                
                plant.PLANT_struct.sun_remainder = 0;
                plant.PLANT_struct.sun_time_counter = 0;   // Reset illumination counter for the next day
            } 
        }
    } 

    // Plant 1, decrement remainder
    if (plant.PLANT_struct.sun_remainder > 0)
    {
        if (--plant.PLANT_struct.sun_remainder > 0)
        {           
            if (do_once_1 == 0)
            {
                expander_i2c.write_bit(EXPANDER_nLIGHT_CTRL_BIT, LIGHT_ON);
                do_once_1 = 1;
            }                
        }
        else
        {
            if (do_once_1 == 1)
            {
                plant.PLANT_struct.sun_remainder = 0;
                expander_i2c.write_bit(EXPANDER_nLIGHT_CTRL_BIT, LIGHT_OFF);
                do_once_1 = 0;
            }
        }
    }              
}

void sensor_refresh_ticker()
{
    // Get sensors value
    adc_i2c.start_then_read_conversion(ADC_LIGHT_INPUT);
    adc_i2c.start_then_read_conversion(ADC_MOISTURE_INPUT);
    adc_i2c.start_then_read_conversion(ADC_TEMPERATURE_INPUT);  
    adc_i2c.start_then_read_conversion(ADC_BATTERY_MONITOR_INPUT);

    // Initiate an ultrasound sequence on HC-SR04P
    // Get the water level
    expander_i2c.write_bit(EXPANDER_TANK_TRIG_BIT, 1);
    delayMicroseconds(20);
    expander_i2c.write_bit(EXPANDER_TANK_TRIG_BIT, 0);        
}

void inactivity_counter_ticker ()
{
    debug_uart_flag = 1;
    if (++intelligrow.inactivity_counter > INACTIVITY_SECOND_COUNTER)
    {
        intelligrow.inactivity_counter = INACTIVITY_SECOND_COUNTER;
        if (intelligrow.inactivity_display_flag == 0)
        {
            intelligrow.inactivity_display_flag = 1;
            intelligrow.menu_screensaver = 1;
        }
    }        
}

void clear_inactivity_counter (void)
{
    intelligrow.inactivity_counter = 0;
    intelligrow.inactivity_display_flag = 0;
    intelligrow.menu_screensaver = 0;
}

void hex_to_ascii (unsigned char ucByte, unsigned char *ucByteH, unsigned char *ucByteL)
{
    *ucByteH = ucByte >> 4;	    //ByteH = Byte d飡l頴 vers droite(val haute)
    *ucByteL = (ucByte & 0x0F);	//ByteL = Byte masqu頥n val basse   
    if ((*ucByteH < 10) && (*ucByteH >= 0)) //Si ByteH inclus entre 0  et 9
    {
        *ucByteH = *ucByteH + 0x30;			//Additionne 30, carac ASCII chiffre
    }
    
    else if ((*ucByteH >= 0x0A) && (*ucByteH <= 0x0F)) //Site ByteH entre 0A et 0F
    {
        *ucByteH = (*ucByteH + 0x37);		//Additionne 37, carac ASCII Lettre
    }
    
    if ((*ucByteL < 10) && (*ucByteL >= 0))	//Si ByteL inclus entre 0  et 9
    {
        *ucByteL = (*ucByteL + 0x30);		//Additionne 30, carac ASCII chiffre
    }
    
    else if ((*ucByteL >= 0x0A) && (*ucByteL <= 0x0F)) //Site ByteL entre 0A et 0F
    {
        *ucByteL = (*ucByteL + 0x37);		//Additionne 37, carac ASCII Lettre
    }   
}

int bcd_to_decimal(unsigned char x) 
{
    return x - 6 * (x >> 4);
}

unsigned int dec2bcd(unsigned int num) // num is now 65535
{
    unsigned int ones = 0;
    unsigned int tens = 0;
    unsigned int temp = 0;

    ones = num%10; // 65535%10 = 5
    temp = num/10; // 65535/10 = 6553
    tens = temp<<4;  // what's displayed is by tens is actually the lower
                     // 4 bits of tens, so tens is 6553%16=9
    return (tens + ones);// so the result is 95
}

unsigned char BCD_to_hex (unsigned char data)
{
    unsigned char temp = 0;
    temp = (data & 0x0F) + ((data & 0xF0)>>4)*10;
    return temp;
}