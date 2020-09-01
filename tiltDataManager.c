#include <stdio.h>
#include <stdlib.h>
#include "string.h"

#include "GUI.h"

#include "wiced_bt_ble.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "tiltDataManager.h"

typedef enum {
    ADD_DATA_POINT,
    GET_DATA_POINT,
} tdm_cmd_t;

typedef struct {
    tdm_cmd_t cmd;
    tdm_tiltHandle_t tilt;
    void *msg;
} tdm_cmdMsg_t;


typedef struct  {
    char *colorName;
    GUI_COLOR color;
    uint8_t uuid[20];
    tdm_tiltData_t *data;
    int numDataPoints;
    int numDataSeen;
} tilt_t;


static QueueHandle_t tdm_cmdQueue;

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


static void tdm_addData(tdm_tiltHandle_t handle, tdm_tiltData_t *data)
{
    if(tiltDB[handle].data != 0)
    {
        free(tiltDB[handle].data);
    }
    tiltDB[handle].data = data; 
    tiltDB[handle].numDataSeen += 1;
    tiltDB[handle].numDataPoints = 1;
}

// This function returns a malloc'd copy of the front of the most recent datapoint ... this function should only be used 
// internally because it is not thread safe.
static tdm_tiltData_t *tdm_getDataPointCopy(tdm_tiltHandle_t handle)
{
    tdm_tiltData_t *dp;
    dp = malloc(sizeof(tdm_tiltData_t));
    memcpy(dp,tiltDB[handle].data,sizeof(tdm_tiltData_t));
    return dp;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Public Functions
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////

void tdm_task(void *arg)
{
    tdm_cmdQueue = xQueueCreate(10,sizeof(tdm_cmdMsg_t));
    tdm_cmdMsg_t msg;
    tdm_tiltData_t *dp;
    tdm_dataRsp_t response;
    while(1)
    {
        xQueueReceive(tdm_cmdQueue,&msg,portMAX_DELAY);
        switch(msg.cmd)
        {
            case ADD_DATA_POINT:
                tdm_addData(msg.tilt,msg.msg);
            break;
            case GET_DATA_POINT:
                dp = tdm_getDataPointCopy(msg.tilt);
                response.response = dp;
                xQueueSend(msg.msg,&response,0); // ARH 0 is probably a bad idea
            break;
        }
    }
}

//
// Should I reall compare all of this data every time?
//
void tdm_processIbeacon(uint8_t *mfgAdvField,int len,wiced_bt_ble_scan_results_t *p_scan_result)
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

            tdm_tiltData_t *data;
            // The tilt repeater will send out 0's if it hasnt heard anything
            if(gravity !=0 && temperature != 0)
            {
                data = malloc(sizeof(tdm_tiltData_t));
                
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


char *tdm_colorString(tdm_tiltHandle_t handle)
{
    return tiltDB[handle].colorName;
}

GUI_COLOR tdm_colorGUI(tdm_tiltHandle_t handle)
{
    return tiltDB[handle].color;
}

int tdm_getNumTilt()
{
    return NUM_TILT;
}

uint32_t tdm_getActiveTiltMask()
{
    uint32_t mask=0;
    for(int i=0;i<NUM_TILT;i++)
    {
        if(tiltDB[i].data)
            mask |= 1<<i;
    }
    return mask;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// These functions submit commands to main command queue: tdm_cmdQueue
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////

void tdm_submitNewData(tdm_tiltHandle_t handle,tdm_tiltData_t *data)
{
    tdm_cmdMsg_t msg;
    msg.msg = data;
    msg.tilt = handle;
    msg.cmd =  ADD_DATA_POINT;
    if(xQueueSend(tdm_cmdQueue,&msg,0) != pdTRUE)
    {
        printf("Failed to send to dmQueue\n");
        free(data);
    }
}

void  tdm_submitGetDataCopy(tdm_tiltHandle_t handle,QueueHandle_t queue)
{
    tdm_cmdMsg_t msg;
    msg.msg = queue;
    msg.tilt = handle;
    msg.cmd =  GET_DATA_POINT;
    if(xQueueSend(tdm_cmdQueue,&msg,0) != pdTRUE)
    {
        printf("failed to send to dmQueue\n");
    }

}
