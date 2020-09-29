#ifndef __menu_manager_h_
    #define __menu_manager_h_

#include "general.h"

#define DATA_TYPE_NUMBER 0
#define DATA_TYPE_LETTER 1

#define CONTEXT_MENU_CHANGE 0
#define CONTEXT_ENTER_DATA 1

#define GRAPH_X_TICK 24
#define GRAPH_Y_TICK 10
#define GRAPH_X_MINOR 3
#define GRAPH_ACC_VALUE (GRAPH_X_TICK*GRAPH_X_MINOR)

// Menu hierarchy
#define SCREENSAVER_MENU 0
    #define MAIN_MENU 1
        #define PLANT_GROWTH_MENU 2
        #define GARDEN_PARAMETERS_MENU 3
        #define GENERAL_SETTINGS_MENU 4

typedef struct
{
    unsigned char actual_menu;
    unsigned char previous_menu;

    unsigned char day_counter;
    unsigned char date_counter;
    unsigned char month_counter;
    unsigned int year_counter;
    unsigned char h_counter;
    unsigned char m_counter;
    unsigned char s_counter;
}MENU_STRUCT;

typedef struct
{
    unsigned int y_min;
    unsigned int y_max;
    unsigned int x_min;
    unsigned int x_max;

    unsigned int zoom_factor;

    unsigned int axis_y_min;
    unsigned int axis_y_max;
    unsigned int axis_y_x_pos;
    unsigned int axis_x_min;
    unsigned int axis_x_max;
    unsigned int axis_x_y_pos;
    unsigned int axis_y_mark;
    unsigned int axis_y_wanted_tick;
    unsigned int axis_x_mark;
    unsigned int axis_x_wanted_tick;

    unsigned int axis_y_legend_x_pos;
    unsigned int axis_y_legend_y_pos;
    char *axis_y_legend_string;
    unsigned int axis_x_legend_x_pos;
    unsigned int axis_x_legend_y_pos;    
    char *axis_x_legend_string;

    unsigned int light_sensor_daily[GRAPH_ACC_VALUE];
    unsigned int temp_sensor_daily[GRAPH_ACC_VALUE];
    unsigned int moist_sensor_daily[GRAPH_ACC_VALUE];  
    unsigned int graph_sensor_pointer;  
    unsigned int graph_update_flag;
}GRAPH_STRUCT;

class MENU_MANAGER
{
    public:
        void init (void);
        void display (unsigned char menu);
        unsigned int y_map (unsigned int y_min, unsigned int y_max, unsigned int var_y_min, unsigned int var_y_max, unsigned int var);
        unsigned int x_map (unsigned int x_min, unsigned int x_max, unsigned int x_mark, unsigned int x_minor, unsigned int var);
        void graphic_handle (void);

        MENU_STRUCT MENU_struct;
        GRAPH_STRUCT GRAPH_struct;
    private:
};

#endif