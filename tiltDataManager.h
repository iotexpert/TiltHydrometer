#pragma once

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "GUI.h"
#include "wiced_bt_ble.h"

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
void tdm_processIbeacon(uint8_t *mfgAdvField,int len,wiced_bt_ble_scan_results_t *p_scan_result);

/////////////// Generally callable threadsafe - non blocking
char *tdm_colorString(tdm_tiltHandle_t handle);    // Return a char * to the color string for the tilt handle
GUI_COLOR tdm_colorGUI(tdm_tiltHandle_t handle);   // Return a GUI_COLOR for the tilt handle
int tdm_getNumTilt();                              // Returns the number of possible tilts (probably always 8)
uint32_t tdm_getActiveTiltMask();                  // Return a bitmask of the active handles


void tdm_submitNewData(tdm_tiltHandle_t handle,tdm_tiltData_t *data);
void tdm_submitGetDataCopy(tdm_tiltHandle_t handle,QueueHandle_t queue);


