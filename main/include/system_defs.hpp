#pragma once

#include "system_enums.hpp"

#define APP_VERSION_MAJOR 1
#define APP_VERSION_MINOR 2
#define APP_VERSION_PATCH 3 // I personally prefer the word "revision" over "patch"

#define ESP_INTR_FLAG_DEFAULT 0

/* System Timer contant */
#define TIMER_PERIOD_100Hz 10000 // 10000 microseconds = .01 second  = 100Hz

/* GPIO Definitions */
#define SW1 GPIO_NUM_0 // Boot Switch -- GPIO_EN.  This a strapping pin is pulled-up by default

/* show */
#define _showInit 0x01
#define _showNVS 0x02
#define _showRun 0x04
#define _showEvents 0x08
#define _showJSONProcessing 0x10
#define _showDebugging 0x20
#define _showProcess 0x40
#define _showPayload 0x80

/* showSys */
#define _showTimerSeconds 0x00000001
#define _showTimerMinutes 0x00000002

/* diagSys */
#define _diagHeapCheck 0x01
#define _printRunTimeStats 0x02
#define _printMemoryStats 0x04
#define _printTaskInfo 0x08
