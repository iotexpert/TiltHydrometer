#pragma once

#include "FreeRTOS.h"
#include "queue.h"

#include "tilt.h"

extern QueueHandle_t dmQueueHandle;


typedef enum {
    dm_addDataPoint,
    dm_getDataPoint,
} dmCmd_t;

typedef struct {
    dmCmd_t cmd;
    tiltHandle_t tilt;
    void *msg;
} dmCmdMsg_t;


typedef struct {
    tilt_data_t *response;
} dm_response_t;



void tdm_dataManagerTask(void *arg);
void tdm_submitNewData(tiltHandle_t handle,tilt_data_t *data);
void tdm_getDataPointCopy(tiltHandle_t handle,QueueHandle_t queue);