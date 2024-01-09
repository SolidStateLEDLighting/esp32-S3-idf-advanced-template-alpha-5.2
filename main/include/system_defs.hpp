#pragma once

#include "system_enums.hpp"

#define APP_VERSION_MAJOR 1
#define APP_VERSION_MINOR 2
#define APP_VERSION_PATCH 3

#define ESP_INTR_FLAG_DEFAULT 0

#define TIMER_PERIOD_1kHz 1000     // 1000 microseconds = .001 second  = 1000Hz
#define TIMER_PERIOD_200Hz 5000    // 5000 microseconds = .005 second  = 200Hz
#define TIMER_PERIOD_100Hz 10000   // 10000 microseconds = .01 second  = 100Hz
#define TIMER_PERIOD_1000Hz 100000 // 1000 microseconds = .001 second  = 1000Hz

#define SW1 GPIO_NUM_0 // Boot Switch -- GPIO_EN.  This a strapping pin is pulled-up by default

//
// Generic show masks to create values from a single byte
//
#define _showInit 0x01
#define _showNVS 0x02
#define _showRun 0x04
#define _showEvents 0x08
#define _showJSONProcessing 0x10
#define _showDebugging 0x20
#define _showProcess 0x40
#define _showPayload 0x80

//
// showSys masks to create values from a 32 bit value
//
#define _showTimerSeconds 0x00000001
#define _showTimerMinutes 0x00000002

//
// diagSys masks to create values from a 32 bit value
//
#define _diagHeapCheck 0x01
#define _printRunTimeStats 0x02
#define _printMemoryStats 0x04
#define _printTaskInfo 0x08
