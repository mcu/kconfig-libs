/******************************************************************************
 * @copyright Copyright (c) Agrolabs LLC. All rights reserved.
 *****************************************************************************/

#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include "FreeRTOS.h"
#include "projdefs.h"
#include "semphr.h"

#if CONFIG_LOG_COLORED
  #define __LOG_COLOR_RED__      "\x1b[31m"
  #define __LOG_COLOR_GREEN__    "\x1b[32m"
  #define __LOG_COLOR_YELLOW__   "\x1b[33m"
  #define __LOG_COLOR_DEFAULT__  "\x1b[0m"
#else
  #define __LOG_COLOR_RED__
  #define __LOG_COLOR_GREEN__
  #define __LOG_COLOR_YELLOW__
  #define __LOG_COLOR_DEFAULT__
#endif

#ifdef FREERTOS
  static SemaphoreHandle_t mutex;
  static StaticSemaphore_t mbuff;
#endif

void log_init()
{
#ifdef FREERTOS
  mutex = xSemaphoreCreateMutexStatic(&mbuff);
#endif
}

void log_info(const char *filename, uint16_t line, const char *fmt, ...)
{
  if((filename == NULL) || (fmt == NULL))
    return;

#ifdef FREERTOS
  if(mutex == NULL)
    return;

  BaseType_t status = xSemaphoreTake(mutex, pdMS_TO_TICKS(10000));
  if(status == pdPASS)
  {
#endif
    va_list args;
    va_start(args, fmt);
    printf(__LOG_COLOR_GREEN__ "[I] %s, %d: ", filename, line);
    vprintf(fmt, args);
    printf(__LOG_COLOR_DEFAULT__ "\r\n");
    va_end(args);
#ifdef FREERTOS
    xSemaphoreGive(mutex);
  }
#endif
}

void log_warning(const char *filename, uint16_t line, const char *fmt, ...)
{
  if((filename == NULL) || (fmt == NULL))
    return;

#ifdef FREERTOS
  if(mutex == NULL)
    return;

  BaseType_t status = xSemaphoreTake(mutex, pdMS_TO_TICKS(10000));
  if(status == pdPASS)
  {
#endif
    va_list args;
    va_start(args, fmt);
    printf(__LOG_COLOR_YELLOW__ "[W] %s, %d: ", filename, line);
    vprintf(fmt, args);
    printf(__LOG_COLOR_DEFAULT__ "\r\n");
    va_end(args);
#ifdef FREERTOS
    xSemaphoreGive(mutex);
  }
#endif
}

void log_error(const char *filename, uint16_t line, const char *fmt, ...)
{
  if((filename == NULL) || (fmt == NULL))
    return;

#ifdef FREERTOS
  if(mutex == NULL)
    return;

  BaseType_t status = xSemaphoreTake(mutex, pdMS_TO_TICKS(10000));
  if(status == pdPASS)
  {
#endif
    va_list args;
    va_start(args, fmt);
    printf(__LOG_COLOR_RED__ "[E] %s, %d: ", filename, line);
    vprintf(fmt, args);
    printf(__LOG_COLOR_DEFAULT__ "\r\n");
    va_end(args);
#ifdef FREERTOS
    xSemaphoreGive(mutex);
  }
#endif
}

/*****************************************************************************/
