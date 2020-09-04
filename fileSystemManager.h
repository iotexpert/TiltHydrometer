#pragma once
#include "tiltDataManager.h"

void fsm_task(void *arg);
void fsm_submitProcessData(tdm_tiltHandle_t handle,tdm_tiltData_t *data);
void fsm_submitPrintData(tdm_tiltHandle_t handle);
