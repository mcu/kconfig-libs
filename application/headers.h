/******************************************************************************
* SPDX-License-Identifier: GPL-3.0-or-later
******************************************************************************/

#pragma once /****************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "projdefs.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"
#include "task.h"
#include "event_groups.h"

#include "stm32f1xx_hal.h"

#include "main.h"
#include "kconfig.h"

#include "log.h"

#ifdef __cplusplus
}
#endif

/*****************************************************************************/
