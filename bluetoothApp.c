
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>

#include "bluetoothApp.h"
#include "wiced_bt_stack.h"
#include "app_bt_cfg.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_trace.h"
#include "tilt.h"

#include "btutil.h"

QueueHandle_t bluetoothAppQueueHandle;

void adv_scan_result_cback(wiced_bt_ble_scan_results_t *p_scan_result, uint8_t *p_adv_data )
{
	if (p_scan_result == 0)
		return;


	uint8_t mfgFieldLen;
	uint8_t *mfgFieldData;
	mfgFieldData = wiced_bt_ble_check_advertising_data(p_adv_data,BTM_BLE_ADVERT_TYPE_MANUFACTURER,&mfgFieldLen);
    
	if(mfgFieldData)
		tilt_processIbeacon(mfgFieldData,mfgFieldLen,p_scan_result);
}

void processBluetoothAppQueue(TimerHandle_t xTimer)
{
	bluetoothAppMsg_t msg;

	 BaseType_t rval;

	 rval = xQueueReceive( bluetoothAppQueueHandle,&msg,0);
	 if(rval == pdTRUE)
	 {
		 switch(msg.cmd)
		 {
		 case BTA_PRINT_TABLE:
			//  dumpTable();
			 break;
		case BTA_PRINT_TABLEFILTER:
			//  dumpTableFilter();
			 break;
		 }
	 }
}


/**************************************************************************************************
* Function Name: app_bt_management_callback()
***************************************************************************************************
* Summary:
*   This is a Bluetooth stack event handler function to receive management events from
*   the BLE stack and process as per the application.
*
* Parameters:
*   wiced_bt_management_evt_t event             : BLE event code of one byte length
*   wiced_bt_management_evt_data_t *p_event_data: Pointer to BLE management event structures
*
* Return:
*  wiced_result_t: Error code from WICED_RESULT_LIST or BT_RESULT_LIST
*
*************************************************************************************************/
wiced_result_t app_bt_management_callback(wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data)
{
    wiced_result_t result = WICED_BT_SUCCESS;


    switch (event)
    {
        case BTM_ENABLED_EVT:

            if (WICED_BT_SUCCESS == p_event_data->enabled.status)
            {
				wiced_bt_ble_observe (WICED_TRUE, 0,adv_scan_result_cback);
            }
            else
            {
            	printf("Error enabling BTM_ENABLED_EVENT\n");
            }

            break;

        default:
            printf("Unhandled Bluetooth Management Event: 0x%x %s\n", event, btutil_getBTEventName(event));
            break;
    }

    return result;
}


void btapp_printTable()
{
	bluetoothAppMsg_t msg;
	msg.cmd = BTA_PRINT_TABLE;
	xQueueSend(bluetoothAppQueueHandle, &msg,0);
}


void btapp_printTableFilter()
{
	bluetoothAppMsg_t msg;
	msg.cmd = BTA_PRINT_TABLEFILTER;
	xQueueSend(bluetoothAppQueueHandle, &msg,0);
}

