#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "tilt.h"

extern QueueHandle_t tdm_cmdQueue;


typedef enum {
    ADD_DATA_POINT,
    GET_DATA_POINT,
} tdm_cmd_t;

typedef struct {
    tdm_cmd_t cmd;
    tiltHandle_t tilt;
    void *msg;
} tdm_cmdMsg_t;


typedef struct {
    tilt_data_t *response;
} tdm_dataRsp_t;



void tdm_task(void *arg);
void tdm_submitNewData(tiltHandle_t handle,tilt_data_t *data);
void  tmd_submitGetDataCopy(tiltHandle_t handle,QueueHandle_t queue);