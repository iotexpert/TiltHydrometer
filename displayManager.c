#include <stdio.h>
#include <stdlib.h>

// #include "cyhal.h"
// #include "cybsp.h"
#include "GUI.h"
#include "mtb_st7789v.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "cy8ckit_028_tft_pins.h"
#include "displayManager.h"
#include "tiltDataManager.h"


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

static QueueHandle_t dataQueue;

QueueHandle_t actionQueue;

static display_screen_name_t currentScreen;

typedef struct {
 display_screen_name_t screenName;
 void (*displayFunction)(void *data);
} display_functionmap_t;

void displayScreenSplash();
void displayScreenTableInit();
void displayScreenTableUpdate();

/* The pins above are defined by the CY8CKIT-028-TFT library. If the display is being used on different hardware the mappings will be different. */
const mtb_st7789v_pins_t tft_pins =
{
    .db08 = CY8CKIT_028_TFT_PIN_DISPLAY_DB8,
    .db09 = CY8CKIT_028_TFT_PIN_DISPLAY_DB9,
    .db10 = CY8CKIT_028_TFT_PIN_DISPLAY_DB10,
    .db11 = CY8CKIT_028_TFT_PIN_DISPLAY_DB11,
    .db12 = CY8CKIT_028_TFT_PIN_DISPLAY_DB12,
    .db13 = CY8CKIT_028_TFT_PIN_DISPLAY_DB13,
    .db14 = CY8CKIT_028_TFT_PIN_DISPLAY_DB14,
    .db15 = CY8CKIT_028_TFT_PIN_DISPLAY_DB15,
    .nrd  = CY8CKIT_028_TFT_PIN_DISPLAY_NRD,
    .nwr  = CY8CKIT_028_TFT_PIN_DISPLAY_NWR,
    .dc   = CY8CKIT_028_TFT_PIN_DISPLAY_DC,
    .rst  = CY8CKIT_028_TFT_PIN_DISPLAY_RST
};

void dm_task(void *arg)
{
    dataQueue = xQueueCreate(10,sizeof(tdm_dataRsp_t));
    actionQueue = xQueueCreate(10,sizeof(dm_action_msg_t));

    /* Initialize the display controller */
    mtb_st7789v_init8(&tft_pins);
    GUI_Init();
    printf("X=%d Y=%d\n",GUI_GetScreenSizeX(),GUI_GetScreenSizeY());

    currentScreen = screen_splash;
    displayScreenSplash();
    displayScreenTableInit();
    for(;;)
    {
        vTaskDelay(5000);
        displayScreenTableUpdate();

    }
}

/////////////// Views ////////////////////


void displayScreenSplash()
{
    printf("Splash\n");

}

            


#define NAME_LEFT_X (0)
#define GRAV_LEFT_X (120)
#define TEMP_LEFT_X (220)

#define NAME_RIGHT_X (119)
#define GRAV_RIGHT_X (219)
#define TEMP_RIGHT_X (319)



#define NAME_CENTER_X (NAME_LEFT_X+(NAME_RIGHT_X-NAME_LEFT_X)/2)
#define GRAV_CENTER_X (GRAV_LEFT_X+(GRAV_RIGHT_X-GRAV_LEFT_X)/2)
#define TEMP_CENTER_X (TEMP_LEFT_X+(TEMP_RIGHT_X-TEMP_LEFT_X)/2)

#define TOP_MARGIN 4
#define LINE_MARGIN 2

#define ROW_Y(row) (TOP_MARGIN + row*(LINE_MARGIN+GUI_GetFontSizeY()))

void displayScreenTableInit()
{

    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLACK);
    GUI_SetFont(GUI_FONT_24B_ASCII);
    GUI_Clear();

    GUI_FillRect(0,0,320,GUI_GetFontSizeY()+ TOP_MARGIN);

    GUI_SetTextMode(GUI_TM_REV);
    GUI_SetTextAlign(GUI_TA_CENTER);
    GUI_DispStringHCenterAt("Name",NAME_CENTER_X,TOP_MARGIN);
    GUI_DispStringHCenterAt("Gravity",GRAV_CENTER_X,TOP_MARGIN);
    GUI_DispStringHCenterAt("Temp",TEMP_CENTER_X,TOP_MARGIN);

    GUI_DrawLine(NAME_RIGHT_X,0,NAME_RIGHT_X,240);
    GUI_DrawLine(GRAV_RIGHT_X,0,GRAV_RIGHT_X,240);
    

}


void displayScreenTableUpdate()
{
    tdm_dataRsp_t myResponse;

    uint32_t activeTilts =tdm_getActiveTiltMask();

    char buff[64];
    
    GUI_SetColor(GUI_WHITE);
    GUI_SetBkColor(GUI_BLACK);
    
    GUI_SetFont(GUI_FONT_24B_ASCII);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    GUI_SetTextAlign(GUI_TA_CENTER);

    int row=1;

    for(int i=0;i<tdm_getNumTilt();i++)
    {
        if(1<<i & activeTilts)
        {
             tdm_submitGetDataCopy(i,dataQueue);
            xQueueReceive(dataQueue,&myResponse,portMAX_DELAY);

            GUI_SetColor(tdm_colorGUI(i));

            GUI_DispStringHCenterAt(tdm_colorString(i), NAME_CENTER_X, ROW_Y(row));
            sprintf(buff,"%1.3f",myResponse.response->gravity);
            GUI_DispStringHCenterAt(buff, GRAV_CENTER_X, ROW_Y(row));
            sprintf(buff,"%02d",myResponse.response->temperature);
            GUI_DispStringHCenterAt(buff, TEMP_CENTER_X, ROW_Y(row));
            
            
            row = row + 1;

#if 0
            printf("Tilt %d Gravity=%f T=%d Time=%ld RSSI=%d txPower=%d\n",
                i,
                myResponse.response->gravity,
                myResponse.response->temperature,
                myResponse.response->time,
                myResponse.response->rssi,
                myResponse.response->txPower
                );
#endif
            free(myResponse.response);
        }
    }
  
}

