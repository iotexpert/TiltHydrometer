#include "tiltDataManager.h"
#include <stdio.h>
#include <stdlib.h>

QueueHandle_t dmQueueHandle;

void tdm_dataManagerTask(void *arg)
{
    dmQueueHandle = xQueueCreate(10,sizeof(dmCmdMsg_t));
    dmCmdMsg_t msg;
    tilt_data_t *dp;
    dm_response_t response;
    while(1)
    {
        xQueueReceive(dmQueueHandle,&msg,portMAX_DELAY);
        switch(msg.cmd)
        {
            case dm_addDataPoint:
                tilt_addData(msg.tilt,msg.msg);
            break;
            case dm_getDataPoint:
                dp = tilt_getDataPointCopy(msg.tilt);
                response.response = dp;
                xQueueSend(msg.msg,&response,0); // ARH 0 is probably a bad idea
            break;
        }
    }
}




////////////////////////////////////////////////////

void tdm_submitNewData(tiltHandle_t handle,tilt_data_t *data)
{
    dmCmdMsg_t msg;
    msg.msg = data;
    msg.tilt = handle;
    msg.cmd =  dm_addDataPoint;
    if(xQueueSend(dmQueueHandle,&msg,0) != pdTRUE)
    {
        printf("failed to send to dmQueue\n");
        free(data);
    }
}

void tdm_getDataPointCopy(tiltHandle_t handle,QueueHandle_t queue)
{
    dmCmdMsg_t msg;
    msg.msg = queue;
    msg.tilt = handle;
    msg.cmd =  dm_getDataPoint;
    if(xQueueSend(dmQueueHandle,&msg,0) != pdTRUE)
    {
        printf("failed to send to dmQueue\n");
    }

}

