#pragma once
#include "tiltDataManager.h"

void displayThread(void *arg);

typedef enum {
    display_refresh,

} dm_action_t;

typedef struct {
    dm_action_t action;
} dm_action_msg_t;


typedef enum {
    screen_splash,
    screen_table,
    screen_single,
} display_screen_name_t;
