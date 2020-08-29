#pragma once
#include <stdint.h>
#include "wiced_bt_ble.h"
#include "GUI.h"

typedef struct {
    float gravity;
    int temperature;
    int8_t rssi;
    int8_t txPower;
	uint32_t time;
} tilt_data_t;

typedef struct  {
    char *colorName;
    GUI_COLOR color;
    uint8_t uuid[20];
    tilt_data_t *data;
    int numDataPoints;
    int numDataSeen;
} tilt_t;

typedef int tiltHandle_t;

void tilt_processIbeacon(uint8_t *mfgAdvField,int len,wiced_bt_ble_scan_results_t *p_scan_result);

void tilt_addData(tiltHandle_t handle, tilt_data_t *data);

char *tilt_colorString(tiltHandle_t handle);
GUI_COLOR tilt_colorGUI(tiltHandle_t handle);


// Return a bitmask of the active handles
uint32_t tilt_getActiveTiltMask();
void tilt_requestActive(tiltHandle_t);

// malloc a copy of the data... the caller is responsible for the free
tilt_data_t *tilt_getDataPointCopy(tiltHandle_t handle);
int tilt_getNumTilt();
