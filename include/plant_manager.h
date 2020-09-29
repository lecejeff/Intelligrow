#ifndef __plant_manager_h_
#define __plant_manager_h_

#include "general.h"

#define PLANT_QTY 3

#define PLANT_1 0
#define PLANT_2 1
#define PLANT_3 2

#define ALARM_ON 1
#define ALARM_OFF 0

#define PLANT_NAME_LENGTH 11

typedef struct
{
    char * name;
    unsigned char sun_time_h;
    unsigned int sun_intensity_real;
    unsigned int temperature_dec;
    float temperature_real;
    unsigned int soil_moisture_real;
    unsigned int sun_intensity_setp;
    unsigned int temperature_setp;
    unsigned int soil_moisture_setp;
    unsigned long sun_time_counter;  
    unsigned char alarm_state;
    unsigned long sun_remainder;
}PLANT_STRUCT;

typedef struct
{
    unsigned char water_time_h;
    unsigned char water_time_m;
    unsigned char sun_light_time_h;
    unsigned char sun_light_time_m;       
    unsigned int water_max_level;
    unsigned int water_min_level;
    unsigned char plant_select;
    unsigned int pump_on_time;
    unsigned int dark_threshold;
    unsigned int soil_dry_threshold;
    unsigned int soil_wet_threshold;
    unsigned char water_plant_trig;
    unsigned char sun_plant_trig;
}GARDEN_STRUCT;

class PLANT_MANAGER
{
    public:
        void init (unsigned char plant);
        void water_alarm (void);
        unsigned char get_alarm_state (unsigned char channel);

        PLANT_STRUCT PLANT_struct;
        GARDEN_STRUCT GARDEN_struct;    
    private:

};
#endif