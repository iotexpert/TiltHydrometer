#include <stdio.h>
#include <stdlib.h>

#include "tiltDataManager.h"
#include "fileSystemManager.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

static QueueHandle_t fsm_cmdQueue;

typedef enum {
    PROCESS_DATA,
    PRINT_DATA,

} fsm_cmd_t;

typedef struct {
    fsm_cmd_t cmd;
    tdm_tiltHandle_t handle;
    tdm_tiltData_t *data;
} fsm_cmdMsg_t;

tdm_tiltData_t *tiltHistory[8]; // bad arh

void fsm_task(void *arg)
{
    printf("Started FileSystem\n");
    fsm_cmdMsg_t msg;
    fsm_cmdQueue = xQueueCreate(20,sizeof(fsm_cmdMsg_t));
    
    memset(tiltHistory,0,sizeof(tiltHistory));

    while(1)
    {
        xQueueReceive(fsm_cmdQueue,&msg,portMAX_DELAY);
        switch(msg.cmd)
        {
            case PROCESS_DATA:
                printf("Got Data %d Grav=%1.3f Temp=%d\n",msg.handle,msg.data->gravity,msg.data->temperature);
                msg.data->next = tiltHistory[msg.handle];
                tiltHistory[msg.handle] = msg.data;
            break;
            case PRINT_DATA:
                printf("History = %d\n",msg.handle);
                for(tdm_tiltData_t *dp =tiltHistory[msg.handle];dp;dp = dp->next)
                {
                    printf("Time=%d Gravity=%1.3f Temperature=%d\n",dp->time,dp->gravity,dp->temperature);
                }
            break;
        }

    }
}


void fsm_submitProcessData(tdm_tiltHandle_t handle,tdm_tiltData_t *data)
{
    fsm_cmdMsg_t msg;
    msg.cmd = PROCESS_DATA;
    msg.handle = handle;
    msg.data = data;
    xQueueSend(fsm_cmdQueue,&msg,portMAX_DELAY);
}

void fsm_submitPrintData(tdm_tiltHandle_t handle)
{
    fsm_cmdMsg_t msg;
    msg.cmd = PRINT_DATA;
    msg.handle = handle;
    xQueueSend(fsm_cmdQueue,&msg,portMAX_DELAY);
}
