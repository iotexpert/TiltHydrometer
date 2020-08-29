#pragma once

#include "FreeRTOS.h"
#include "queue.h"
#include "GUI.h"

#include "wiced_bt_ble.h"

extern QueueHandle_t tdm_cmdQueue;

typedef struct {
    float gravity;
    int temperature;
    int8_t rssi;
    int8_t txPower;
	uint32_t time;
} tilt_data_t;

typedef struct {
    tilt_data_t *response;
} tdm_dataRsp_t;



typedef int tiltHandle_t;


void tdm_task(void *arg);
void tdm_submitNewData(tiltHandle_t handle,tilt_data_t *data);
void  tmd_submitGetDataCopy(tiltHandle_t handle,QueueHandle_t queue);



void tilt_processIbeacon(uint8_t *mfgAdvField,int len,wiced_bt_ble_scan_results_t *p_scan_result);
void tilt_addData(tiltHandle_t handle, tilt_data_t *data);

// Return a bitmask of the active handles
uint32_t tilt_getActiveTiltMask();
void tilt_requestActive(tiltHandle_t);

// malloc a copy of the data... the caller is responsible for the free
tilt_data_t *tilt_getDataPointCopy(tiltHandle_t handle);

/////////////// Generally callable threadsafe

int tilt_getNumTilt();
char *tilt_colorString(tiltHandle_t handle);
GUI_COLOR tilt_colorGUI(tiltHandle_t handle);
