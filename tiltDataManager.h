#pragma once
#include <stdint.h>

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
} tdm_tiltData_t;

typedef struct {
    tdm_tiltData_t *response;
} tdm_dataRsp_t;

typedef int tdm_tiltHandle_t;

void tdm_task(void *arg);

void tdm_submitNewData(tdm_tiltHandle_t handle,tdm_tiltData_t *data);
void tdm_submitGetDataCopy(tdm_tiltHandle_t handle,QueueHandle_t queue);

/// Called by the Bluetooth App when it finds new iBeacons... will submit to the queue
void tdm_processIbeacon(uint8_t *mfgAdvField,int len,wiced_bt_ble_scan_results_t *p_scan_result);


/////////////// Generally callable threadsafe - non blocking
uint32_t tdm_getActiveTiltMask();              // Return a bitmask of the active handles
int tdm_getNumTilt();                          // Returns the number of tilts (probably always 8)
char *tdm_colorString(tdm_tiltHandle_t handle);    // Return a char * to the color string for the tilt
GUI_COLOR tdm_colorGUI(tdm_tiltHandle_t handle);   // Return a GUI_COLOR for the tilt
