#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "tilt.h"

extern QueueHandle_t tdm_cmdQueue;


typedef struct {
    tilt_data_t *response;
} tdm_dataRsp_t;

void tdm_task(void *arg);
void tdm_submitNewData(tiltHandle_t handle,tilt_data_t *data);
void  tmd_submitGetDataCopy(tiltHandle_t handle,QueueHandle_t queue);