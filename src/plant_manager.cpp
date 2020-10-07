#include "plant_manager.h"
#include "rtc.h"

extern RTC_class rtc;

void PLANT_MANAGER::init (unsigned char plant)
{
    GARDEN_struct.pump_on_time = 2;                                         // Number of seconds the pump are activated
    GARDEN_struct.water_full_level = 7;                                      // Water tank max level
    GARDEN_struct.water_empty_level = 23;                                     // Water tank min level

    GARDEN_struct.water_time_h = 8;                                         // Time at which we should water the plants, for the pump_on_time amount of time
    GARDEN_struct.water_time_m = 0;

    switch (plant)
    {
        case PLANT_1:
            PLANT_struct.name = "Basilic reg";                         // Plant name
            PLANT_struct.sun_time_h = 12;                              // Amount of sunlight hours required / day, starting at 8 AM
            PLANT_struct.soil_moisture_setp = 200;                     // Soil moisture setpoint
            PLANT_struct.sun_intensity_setp = 500;                     // Light intensity setpoint
            PLANT_struct.temperature_setp = 20;                        //20.5
            PLANT_struct.alarm_state = 0;                              // No pending alarm
            PLANT_struct.sun_time_counter = 0;                         // Reset sun time counter
            PLANT_struct.sun_remainder = 1;                            // Reset sun time remainder
            break;
    }
}