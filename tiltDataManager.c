#include "tiltDataManager.h"
#include <stdio.h>
#include <stdlib.h>

QueueHandle_t tdm_cmdQueue;

typedef enum {
    ADD_DATA_POINT,
    GET_DATA_POINT,
} tdm_cmd_t;

typedef struct {
    tdm_cmd_t cmd;
    tiltHandle_t tilt;
    void *msg;
} tdm_cmdMsg_t;


void tdm_task(void *arg)
{
    tdm_cmdQueue = xQueueCreate(10,sizeof(tdm_cmdMsg_t));
    tdm_cmdMsg_t msg;
    tilt_data_t *dp;
    tdm_dataRsp_t response;
    while(1)
    {
        xQueueReceive(tdm_cmdQueue,&msg,portMAX_DELAY);
        switch(msg.cmd)
        {
            case ADD_DATA_POINT:
                tilt_addData(msg.tilt,msg.msg);
            break;
            case GET_DATA_POINT:
                dp = tilt_getDataPointCopy(msg.tilt);
                response.response = dp;
                xQueueSend(msg.msg,&response,0); // ARH 0 is probably a bad idea
            break;
        }
    }
}




////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// These functions submit commands to main command queue: tdm_cmdQueue
// 
////////////////////////////////////////////////////////////////////////////////////////////////////////

void tdm_submitNewData(tiltHandle_t handle,tilt_data_t *data)
{
    tdm_cmdMsg_t msg;
    msg.msg = data;
    msg.tilt = handle;
    msg.cmd =  ADD_DATA_POINT;
    if(xQueueSend(tdm_cmdQueue,&msg,0) != pdTRUE)
    {
        printf("failed to send to dmQueue\n");
        free(data);
    }
}

void  tmd_submitGetDataCopy(tiltHandle_t handle,QueueHandle_t queue)
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

