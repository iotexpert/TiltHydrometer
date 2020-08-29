#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

#include "tilt.h"
#include "tiltDataManager.h"
#include "GUI.h"


typedef struct  {
    char *colorName;
    GUI_COLOR color;
    uint8_t uuid[20];
    tilt_data_t *data;
    int numDataPoints;
    int numDataSeen;
} tilt_t;


#define IBEACON_HEADER 0x4C,0x00,0x02,0x15

#define GUI_PINK GUI_MAKE_COLOR(0x00CCCCFF)
#define GUI_PURPLE GUI_MAKE_COLOR(0x00800080)

static tilt_t tiltDB [] =
{
    {"Red",    GUI_RED,    {IBEACON_HEADER,0xA4,0x95,0xBB,0x10,0xC5,0xB1,0x4B,0x44,0xB5,0x12,0x13,0x70,0xF0,0x2D,0x74,0xDE},0,0,0},
    {"Green" , GUI_GREEN,  {IBEACON_HEADER,0xA4,0x95,0xBB,0x20,0xC5,0xB1,0x4B,0x44,0xB5,0x12,0x13,0x70,0xF0,0x2D,0x74,0xDE},0,0,0},
    {"Black" , GUI_GRAY,   {IBEACON_HEADER,0xA4,0x95,0xBB,0x30,0xC5,0xB1,0x4B,0x44,0xB5,0x12,0x13,0x70,0xF0,0x2D,0x74,0xDE},0,0,0},
    {"Purple", GUI_PURPLE, {IBEACON_HEADER,0xA4,0x95,0xBB,0x40,0xC5,0xB1,0x4B,0x44,0xB5,0x12,0x13,0x70,0xF0,0x2D,0x74,0xDE},0,0,0},
    {"Orange", GUI_ORANGE, {IBEACON_HEADER,0xA4,0x95,0xBB,0x50,0xC5,0xB1,0x4B,0x44,0xB5,0x12,0x13,0x70,0xF0,0x2D,0x74,0xDE},0,0,0},
    {"Blue"  , GUI_BLUE,   {IBEACON_HEADER,0xA4,0x95,0xBB,0x60,0xC5,0xB1,0x4B,0x44,0xB5,0x12,0x13,0x70,0xF0,0x2D,0x74,0xDE},0,0,0},
    {"Yellow", GUI_YELLOW, {IBEACON_HEADER,0xA4,0x95,0xBB,0x70,0xC5,0xB1,0x4B,0x44,0xB5,0x12,0x13,0x70,0xF0,0x2D,0x74,0xDE},0,0,0},
    {"Pink"  , GUI_PINK,   {IBEACON_HEADER,0xA4,0x95,0xBB,0x80,0xC5,0xB1,0x4B,0x44,0xB5,0x12,0x13,0x70,0xF0,0x2D,0x74,0xDE},0,0,0},
};

#define NUM_TILT (sizeof(tiltDB)/sizeof(tilt_t))


//
// Should I reall compare all of this data every time?
//
void tilt_processIbeacon(uint8_t *mfgAdvField,int len,wiced_bt_ble_scan_results_t *p_scan_result)
{
    if(len != 25)
        return;

    for(int i=0;i<NUM_TILT;i++)
    {
        if(memcmp(mfgAdvField,tiltDB[i].uuid,20) == 0)
        {
            uint32_t timerTime = xTaskGetTickCount() / 1000;
		    int8_t txPower = mfgAdvField[24];
		    float gravity = ((float)((uint16_t)mfgAdvField[22] << 8 | (uint16_t)mfgAdvField[23]))/1000;
		    int temperature = mfgAdvField[20] << 8 | mfgAdvField[21];

            tilt_data_t *data;
            // The tilt repeater will send out 0's if it hasnt heard anything
            if(gravity !=0 && temperature != 0)
            {
                data = malloc(sizeof(tilt_data_t));
                
                data->gravity     = gravity;
                data->temperature = temperature;
                data->txPower     = txPower;
                data->time        = timerTime;
                data->rssi        = p_scan_result->rssi;

                tdm_submitNewData(i,data);

            }

        }
    }
}

char *tilt_colorString(tiltHandle_t handle)
{
    return tiltDB[handle].colorName;
}

GUI_COLOR tilt_colorGUI(tiltHandle_t handle)
{
    return tiltDB[handle].color;
}

void tilt_addData(tiltHandle_t handle, tilt_data_t *data)
{
    if(tiltDB[handle].data != 0)
    {
        free(tiltDB[handle].data);
    }
    tiltDB[handle].data = data; 
    tiltDB[handle].numDataSeen += 1;
    tiltDB[handle].numDataPoints = 1;
}

int tilt_getNumTilt()
{
    return NUM_TILT;
}

uint32_t tilt_getActiveTiltMask()
{
    uint32_t mask=0;
    for(int i=0;i<NUM_TILT;i++)
    {
        if(tiltDB[i].data)
            mask |= 1<<i;
    }
    return mask;
}

tilt_data_t *tilt_getDataPointCopy(tiltHandle_t handle)
{
    tilt_data_t *dp;
    dp = malloc(sizeof(tilt_data_t));
    memcpy(dp,tiltDB[handle].data,sizeof(tilt_data_t));
    return dp;

}