#pragma once

#include <stddef.h> // Standard libraries
#include <stdint.h>
#include <string>

#include "freertos/FreeRTOS.h" // RTOS Libraries
#include "freertos/queue.h"

//
// Run Notifications and Commands
//
// Task Notifications should be used for notifications or commands which need no input and return no data.
enum class SYS_NOTIFY : uint8_t // Task Notification definitions for the Run loop
{
    NONE = 0,
};

// Queue based commands should be used for commands which may provide input and perhaps return data.
enum class SYS_COMMAND : uint8_t
{
    NONE = 0,
};

//
// Message Operations
//
enum class SYS_STATUS : uint8_t
{
    DATA_OK,
    DATA_EMPTY,
    ERROR,
};

struct SYS_CmdRequest
{
    QueueHandle_t queueToSendResponse; // 4 bytes.   If NULL, no response will be sent.
    SYS_COMMAND requestedCmd;
    std::string *stringData = nullptr;
    int64_t data64bit = 0;
};

struct SYS_Response
{
    SYS_STATUS responseCode;
    std::string *jsonResponse = nullptr;
};

//
// Class Operations
//
enum class SYS_OP : uint8_t
{
    Run,
    Init,
    Error,
    Idle,
    Idle_Silent,
};

enum class SYS_INIT : uint8_t
{
    Start,
    Init_Queues_Commands,
    Create_Default_Event_Loop,
    Start_Network_Interface,
    Create_Indication,
    Wait_On_Indication,
    Finished,
    Error,
};

//
// Logging
//
enum class LOG_TYPE : uint8_t
{
    ERROR = 1,
    WARN,
    INFO,
};