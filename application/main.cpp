/******************************************************************************
* SPDX-License-Identifier: GPL-3.0-or-later
******************************************************************************/

#include "headers.h"

static StackType_t tstack[512];
static StaticTask_t stask;

/* Redirect printf to ITM channel 0 */
extern "C" int32_t __io_putchar(int32_t ch)
{
  ITM_SendChar(ch);

  return ch;
}

void main_led_task(void *args __attribute__((unused)))
{
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

    vTaskDelay(pdMS_TO_TICKS(CONFIG_APP_BLINK_LED_DELAY));
  }
  vTaskDelete(NULL);
}

extern "C" void main_app()
{
  LOGI("OS: FreeRTOS %s", tskKERNEL_VERSION_NUMBER);
  LOGI("FLASH: page size '%d' bytes", FLASH_PAGE_SIZE);

  xTaskCreateStatic(main_led_task, "led", sizeof(tstack)/sizeof(tstack[0]), NULL, FREERTOS_TASK_PRIORITY_MEDIUM, tstack, &stask);

  vTaskStartScheduler();
}

/*****************************************************************************/
