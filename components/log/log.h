/******************************************************************************
 * @copyright Copyright (c) Agrolabs LLC. All rights reserved.
 *****************************************************************************/

#pragma once /****************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void log_init();

#ifdef DEBUG
  #define LOGI(FMT, ...) log_info(__FILE_NAME__, __LINE__, FMT, ##__VA_ARGS__)
  #define LOGW(FMT, ...) log_warning(__FILE_NAME__, __LINE__, FMT, ##__VA_ARGS__)
  #define LOGE(FMT, ...) log_error(__FILE_NAME__, __LINE__, FMT, ##__VA_ARGS__)

  void log_info(const char *filename, uint16_t line, const char *fmt, ...);
  void log_warning(const char *filename, uint16_t line, const char *fmt, ...);
  void log_error(const char *filename, uint16_t line, const char *fmt, ...);

#else //release
  #define LOGI(TAG, FMT, ...) {}
  #define LOGW(TAG, FMT, ...) {}
  #define LOGE(TAG, FMT, ...) {}
#endif

#ifdef __cplusplus
}
#endif

/*****************************************************************************/
