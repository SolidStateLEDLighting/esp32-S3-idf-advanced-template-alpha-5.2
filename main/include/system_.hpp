#pragma once

#include "sdkconfig.h"     // Configuration variables
#include "system_defs.hpp" // Local definitions, structs, and enumerations

#include <stdio.h> // Standard libraries
#include <inttypes.h>
#include <stdbool.h>
#include <sstream>
#include <memory>

#include "freertos/task.h" // RTOS libraries (remaining files)
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "freertos/FreeRTOSConfig.h"

#include <driver/gpio.h> // IDF components
#include <esp_log.h>
#include <esp_err.h>
#include "esp_event.h"
#include "esp_timer.h"
#include "esp_sntp.h"
#include "esp_log.h"

#include "nvs/nvs_.hpp" // Our components
#include "indication/indication_.hpp"
#include "wifi/wifi_.hpp"

class NVS; // Forward declarations
class Indication;
class Wifi;

extern "C"
{
    class System
    {
    public:
        /* System_ */
        static System *getInstance() // Enforce use of System as a singleton object
        {
            static System sysInstance;
            return &sysInstance;
        }

        TaskHandle_t get_runTaskHandle(void);
        QueueHandle_t get_CmdRequestQueue(void);

    private:
        /* System_ */
        char TAG[5] = "_sys";

        /* Object States */
        WIFI_CONN_STATE sysWifiConnState = WIFI_CONN_STATE::WIFI_DISCONNECTED;

        /* Object References */
        NVS *nvs = nullptr;
        Indication *ind = nullptr;
        Wifi *wifi = nullptr;

        uint8_t show = 0;      // Flags
        uint8_t showSys = 0;   //
        uint8_t diagSys = 0;   //
        uint16_t iotDirty = 0; //

        uint32_t bootCount = 0;

        QueueHandle_t queHandleWIFICmdRequest = nullptr;
        QueueHandle_t queHandleIndCmdRequest = nullptr;

        uint8_t runStackSizeK = 6;                  // Default minimum size
        TaskHandle_t taskHandleSystemRun = nullptr; //

        uint8_t gpioStackSizeK = 3;                     // Default minimum size
        TaskHandle_t runTaskHandleSystemGPIO = nullptr; //

        uint8_t timerStackSizeK = 4;                     // Default minimum size
        TaskHandle_t taskHandleRunSystemTimer = nullptr; //

        System(void);
        System(const System &) = delete;         // Disable copy constructor
        void operator=(System const &) = delete; // Disable assignment operator

        void setFlags(void);
        void setLogLevels(void);
        void createSemaphores(void);
        void setConditionalCompVariables(void);

        /* System_Diagnostics */
        void printRunTimeStats(void);
        void printMemoryStats();
        void printTaskInfo(void);

        /* System_gpio */
        void initGPIOPins(void);
        void initGPIOTask(void);
        static void runGPIOTaskMarshaller(void *);
        void runGPIOTask(void); // Handles GPIO Interrupts on Change Events

        /* System_Logging */
        std::string errMsg = "";

        void routeLogByRef(LOG_TYPE, std::string *);
        void routeLogByValue(LOG_TYPE, std::string);

        /* System_NVS */
        bool saveToNVSFlag = false;
        uint8_t saveToNVSDelaySecs = 0;

        bool restoreVariablesFromNVS(void);
        bool saveVariablesToNVS(void);

        /* System_Run */
        SYS_NOTIFY sysTaskNotifyValue = SYS_NOTIFY::NONE;

        QueueHandle_t systemCmdRequestQue = nullptr; // Command Queue
        SYS_CmdRequest *ptrSYSCmdRequest = nullptr;
        SYS_Response *ptrSYSResponse = nullptr;
        std::string strCmdPayload = "";

        SYS_OP sysOP = SYS_OP::Idle; // State variables
        SYS_OP opSys_Return = SYS_OP::Idle;
        SYS_INIT initSysStep = SYS_INIT::Finished;

        TaskHandle_t taskHandleIndRun = nullptr; // RTOS
        TaskHandle_t taskHandleSNTPRun = nullptr;
        TaskHandle_t taskHandleWIFIRun = nullptr;
        TaskHandle_t taskHandleMeshRun = nullptr;

        static void runMarshaller(void *); // Handles all System activites
        void run(void);                    //

        /* System_Timer */
        uint8_t rebootTimerSec = 0;
        uint8_t syncEventTimeOut_Counter = 0;
        esp_timer_handle_t handleGenTimer = nullptr;

        static void runGenTimerTaskMarshaller(void *); // Handles all Timer related events
        void runGenTimerTask(void);                    //

        void initGenTimer(void);
        static void genTimerCallback(void *);

        void halfSecondActions(void);
        void oneSecondActions(void);
        void fiveSecondActions(void);
        void tenSecondActions(void);
        void oneMinuteActions(void);
        void fiveMinuteActions(void);

        /* Utilities */
        const char *convertWifiStateToChars(uint8_t);
        std::string getDeviceID(void);

        uint8_t lockGetUint8(uint8_t *);                     // Locking uint8_t variables
        void lockOrUint8(uint8_t *, uint8_t);                //
        void lockAndUint8(uint8_t *variable, uint8_t value); //
        void lockSetUint8(uint8_t *, uint8_t);               //
    };
}