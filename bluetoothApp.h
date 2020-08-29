#include "FreeRTOS.h"
#include "wiced_bt_stack.h"
#include "wiced_bt_dev.h"
#include "queue.h"

wiced_result_t app_bt_management_callback(wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data);

typedef enum {
	BTA_PRINT_TABLE,
	BTA_PRINT_TABLEFILTER,

} bluetoothAppCmd_t;

typedef struct {
	bluetoothAppCmd_t cmd;

} bluetoothAppMsg_t;

extern QueueHandle_t bluetoothAppQueueHandle;

void btapp_printTable();
void btapp_printTableFilter();
