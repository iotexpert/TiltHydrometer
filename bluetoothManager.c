
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

#include "bluetoothManager.h"
#include "wiced_bt_stack.h"
#include "app_bt_cfg.h"
#include "wiced_bt_dev.h"
#include "wiced_bt_trace.h"

//#include "btutil.h"
#include "tiltDataManager.h"

QueueHandle_t btm_cmdQueue;

typedef enum {
	BTA_NONE,
} btm_cmd_t;

typedef struct {
	btm_cmd_t cmd;

} btm_cmdMsg_t;


static void btm_advScanResultCback(wiced_bt_ble_scan_results_t *p_scan_result, uint8_t *p_adv_data )
{
	if (p_scan_result == 0)
		return;


	uint8_t mfgFieldLen;
	uint8_t *mfgFieldData;
	mfgFieldData = wiced_bt_ble_check_advertising_data(p_adv_data,BTM_BLE_ADVERT_TYPE_MANUFACTURER,&mfgFieldLen);
    
	if(mfgFieldData)
		tdm_processIbeacon(mfgFieldData,mfgFieldLen,p_scan_result);
}

#if 0
/// Currently never called
static void btm_processCmdQueue(TimerHandle_t xTimer)
{
	btm_cmdMsg_t msg;
	BaseType_t rval;

	rval = xQueueReceive( btm_cmdQueue,&msg,0);
	if(rval == pdTRUE)
	{
		switch(msg.cmd)
		{
			case BTA_NONE:
		 	printf("BTA recievec command BTA_NONE\n");
			break;
		 }
	 }
}
#endif

/**************************************************************************************************
* Function Name: btm_bteManagementCallback()
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
wiced_result_t btm_bteManagementCallback(wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data)
{
    wiced_result_t result = WICED_BT_SUCCESS;


    switch (event)
    {
        case BTM_ENABLED_EVT:

            if (WICED_BT_SUCCESS == p_event_data->enabled.status)
            {
				wiced_bt_ble_observe(WICED_TRUE, 0,btm_advScanResultCback);
            }
            else
            {
            	printf("Error enabling BTM_ENABLED_EVENT\n");
            }

            break;

        default:
            //printf("Unhandled Bluetooth Management Event: 0x%x %s\n", event, btutil_getBTEventName(event));
            break;
    }

    return result;
}
