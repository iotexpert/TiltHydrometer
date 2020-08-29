
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>

#include "bluetoothApp.h"
#include "wiced_bt_stack.h"
#include "app_bt_cfg.h"
#include "wiced_bt_dev.h"

#include "displayThread.h"
#include "tiltDataManager.h"

volatile int uxTopUsedPriority ;
TaskHandle_t blinkTaskHandle;



// You need these includes

#include "ntshell.h"
#include "ntlibc.h"
#include "psoc6_ntshell_port.h"

// Global variable with a handle to the shell
ntshell_t ntshell;

void ntShellTask()
{

  printf("Started ntshell\n");
  setvbuf(stdin, NULL, _IONBF, 0);
  ntshell_init(
	       &ntshell,
	       ntshell_read,
	       ntshell_write,
	       ntshell_callback,
	       (void *)&ntshell);
  ntshell_set_prompt(&ntshell, "Tilt Sensor>");
  vtsend_erase_display(&ntshell.vtsend);
  ntshell_execute(&ntshell);
}

// this will start the task

void blinkTask(void *arg)
{
    cyhal_gpio_init(CYBSP_USER_LED,CYHAL_GPIO_DIR_OUTPUT,CYHAL_GPIO_DRIVE_STRONG,0);

    for(;;)
    {
    	cyhal_gpio_toggle(CYBSP_USER_LED);
    	vTaskDelay(500);
    }
}


int main(void)
{
    uxTopUsedPriority = configMAX_PRIORITIES - 1 ; // enable OpenOCD Thread Debugging

    /* Initialize the device and board peripherals */
    cybsp_init() ;

    __enable_irq();

    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("Started Application\n");

    cybt_platform_config_init(&bt_platform_cfg_settings);
    wiced_bt_stack_init (app_bt_management_callback, &wiced_bt_cfg_settings);

    // Stack size in WORDs
    // Idle task = priority 0
    xTaskCreate(blinkTask, "Blink", configMINIMAL_STACK_SIZE,0 /* args */ ,0 /* priority */, &blinkTaskHandle);
    xTaskCreate(ntShellTask, "NT Shell", configMINIMAL_STACK_SIZE*4,0 /* args */ ,0 /* priority */, 0);
    xTaskCreate(displayThread, "Display Manager", configMINIMAL_STACK_SIZE*3,0 /* args */ ,0 /* priority */, 0);
    xTaskCreate(tdm_task, "Tile Data Manager", configMINIMAL_STACK_SIZE*2,0 /* args */ ,0 /* priority */, 0);

    vTaskStartScheduler();


}

/* [] END OF FILE */
