#include <stdio.h>

#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"

#include "FreeRTOS.h"
#include "task.h"

#include "bluetoothManager.h"
#include "wiced_bt_stack.h"
#include "app_bt_cfg.h"
#include "wiced_bt_dev.h"

#include "displayManager.h"
#include "tiltDataManager.h"

#include "ntshell.h"
#include "ntlibc.h"
#include "psoc6_ntshell_port.h"

volatile int uxTopUsedPriority ;

ntshell_t nts_shell;

void nts_task()
{
  printf("Started ntshell\n");
  ntshell_init(
	       &nts_shell,
	       ntshell_read,
	       ntshell_write,
	       ntshell_callback,
	       (void *)&nts_shell);
  ntshell_set_prompt(&nts_shell, "Tilt Sensor> ");
  vtsend_erase_display(&nts_shell.vtsend);
  ntshell_execute(&nts_shell);
}

void blink_task(void *arg)
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

    cybsp_init() ;
    __enable_irq();

    cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("Started Application\n");

    cybt_platform_config_init(&bt_platform_cfg_settings);
    wiced_bt_stack_init (btm_bteManagementCallback, &wiced_bt_cfg_settings);

    // Stack size in WORDs
    // Idle task = priority 0
    xTaskCreate(blink_task, "Blink", configMINIMAL_STACK_SIZE,0 /* args */ ,0 /* priority */, 0);
    xTaskCreate(nts_task, "NT Shell", configMINIMAL_STACK_SIZE*4,0 /* args */ ,0 /* priority */, 0);
    xTaskCreate(dm_task, "Display Manager", configMINIMAL_STACK_SIZE*3,0 /* args */ ,0 /* priority */, 0);
    xTaskCreate(tdm_task, "Tile Data Manager", configMINIMAL_STACK_SIZE*2,0 /* args */ ,0 /* priority */, 0);

    vTaskStartScheduler();
}
