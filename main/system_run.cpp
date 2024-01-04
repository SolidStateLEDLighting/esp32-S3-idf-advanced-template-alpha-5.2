#include "system_.hpp"

#include "esp_check.h"
#include "esp_heap_caps.h"

/* External Variables */
extern SemaphoreHandle_t semIndEntry;
extern SemaphoreHandle_t semSNTPEntry;
extern SemaphoreHandle_t semWifiEntry;
extern SemaphoreHandle_t semMeshEntry;
extern SemaphoreHandle_t semSysRouteLock;
extern SemaphoreHandle_t semSysUint8Lock;

void System::runMarshaller(void *arg)
{
    ((System *)arg)->run();
    vTaskDelete(NULL);
}

void System::run(void)
{
    esp_err_t ret;

    while (true)
    {
        switch (sysOP)
        {
        case SYS_OP::Run:
        {
            /*  Service all Task Notifications */
            /* Task Notifications should be used for notifications or commands which need no input and return no data. */
            sysTaskNotifyValue = static_cast<SYS_NOTIFY>(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(95))); // Wait 30 ticks for any message then move on.

            if (sysTaskNotifyValue > SYS_NOTIFY::NONE)
            {
                switch (sysTaskNotifyValue)
                {
                case SYS_NOTIFY::NONE:
                {
                    if unlikely (show & _showRun)
                        ESP_LOGW(TAG, "SYS_NOTIFY::NONE");
                    break;
                }
                }
            }

            /* Service all Incoming Commands */
            /* Queue based commands should be used for commands which may provide input and perhaps return data. */
            if (xQueuePeek(systemCmdRequestQue, (void *)&ptrSYSCmdRequest, pdMS_TO_TICKS(95))) // We cycle through here and look for incoming mail box command requests
            {
                if (ptrSYSCmdRequest->stringData != nullptr)
                {
                    strCmdPayload = *ptrSYSCmdRequest->stringData; // We should always try to copy the payload even if we don't use that payload.
                    // ESP_LOGI(TAG, "%s", strCmdPayload.c_str());
                }

                switch (ptrSYSCmdRequest->requestedCmd)
                {
                case SYS_COMMAND::NONE:
                {
                    if unlikely (show & _showRun)
                        ESP_LOGI(TAG, "Received the command SYS_COMMAND::NONE");
                    break;
                }
                }

                xQueueReceive(systemCmdRequestQue, (void *)&ptrSYSCmdRequest, pdMS_TO_TICKS(10));
            }

            /* Pending actions */
            if (saveToNVSFlag)
            {
                saveToNVSFlag = false;
                saveVariablesToNVS();
            }

            //
            // Did we connect before -- Yes, then are we the root of the network?  No, then try to find a router.
            //
            // Do we have router1 and/or router2 ssid and passwds?    Favor the router with proven info - favor router1
            //
            // Test routers -- start and connect (has Internet)
            //
            // Has connection ssid/passwrd
            // Connect      -- logs into router
            // HasIPAddress -- can see Internet
            //
            // Are we the root?  Yes, connect.   No, disconnect.
            //

            if (wifiRestart)
            {
                wifiRestart = false;

                // if (wifi != nullptr)
                //     delete wifi;

                // initSysStep = SYS_INIT::Create_Wifi;
                // sysOP = SYS_OP::Init;
            }

            if (diagSys) // We may run periodic or commanded diagnostics
            {
                if (diagSys & _diagHeapCheck)
                {
                    lockAndUint8(&diagSys, _diagHeapCheck); // Clear the bit
                    heap_caps_check_integrity_all(true);    // Esp library test
                }
                else if (diagSys & _printRunTimeStats)
                {
                    lockAndUint8(&diagSys, _printRunTimeStats); // Clear the bit
                    printRunTimeStats();                        // This diagnostic will affect your process over a 45 seconds period.  Can't use without special Menuconfig settings set.
                }
                else if (diagSys & _printRunTimeStats)
                {
                    lockAndUint8(&diagSys, _printMemoryStats); // Clear the bit
                    printMemoryStats();
                }
                else if (diagSys & _printRunTimeStats)
                {
                    lockAndUint8(&diagSys, _printTaskInfo); // Clear the bit
                    printTaskInfo();
                }
            }

            break;
        }

        case SYS_OP::Init:
        {
            switch (initSysStep)
            {
            case SYS_INIT::Start:
            {
                if (show & _showInit)
                    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_INIT::Start");

                initSysStep = SYS_INIT::Init_Queues_Commands;
                [[fallthrough]];
            }

            case SYS_INIT::Init_Queues_Commands:
            {
                if (show & _showInit)
                    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_INIT::Init_Queues_Commands - Step " + std::to_string((int)SYS_INIT::Init_Queues_Commands));

                initSysStep = SYS_INIT::Create_Default_Event_Loop;
                break;
            }

            case SYS_INIT::Create_Default_Event_Loop:
            {
                if (show & _showInit)
                    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_INIT::Create_Default_Event_Loop - Step " + std::to_string((int)SYS_INIT::Create_Default_Event_Loop));

                ret = esp_event_loop_create_default();

                if unlikely (ret != ESP_OK)
                {
                    ESP_LOGE(TAG, "esp_event_loop_create_default() Failed. Error: 0x%X", ret);
                    initSysStep = SYS_INIT::Finished;
                }
                initSysStep = SYS_INIT::Start_Network_Interface;
                break;
            }

            case SYS_INIT::Start_Network_Interface:
            {
                if (show & _showInit)
                    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_INIT::StartNetworkInterface: - Step 5");

                // ESP_GOTO_ON_ERROR(esp_netif_init(), sys_Start_Network_Interface_err, TAG, "esp_netif_init() failure."); // Network Interface initialization - starts up the TCP/IP stack.
                initSysStep = SYS_INIT::Finished;
                break;

                // sys_Start_Network_Interface_err:
                //     errMsg = std::string(__func__) + "(): SYS_INIT::StartNetworkInterface: error: " + esp_err_to_name(ret);
                //     initSysStep = SYS_INIT::Error;
                //     break;
            }

            case SYS_INIT::Finished:
            {
                if (show & _showInit)
                    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_INIT::Finished");

                initGenTimer(); // Starting General Timer task
                sysOP = SYS_OP::Run;
                break;
            }

            case SYS_INIT::Error:
            {
                sysOP = SYS_OP::Error;
                break;
            }
            }
            break;
        }

        case SYS_OP::Error:
        {
            ESP_LOGE(TAG, "Error...");
            sysOP = SYS_OP::Idle;
            break;
        }

        case SYS_OP::Idle:
        {
            ESP_LOGW(TAG, "Idle...");
            sysOP = SYS_OP::Idle_Silent;
            [[fallthrough]];
        }

        case SYS_OP::Idle_Silent:
        {
            vTaskDelay(pdMS_TO_TICKS(5000));
            break;
        }
        }
        taskYIELD();
    }
}