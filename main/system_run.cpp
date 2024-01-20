#include "system_.hpp"

#include "esp_netif.h"
#include "esp_check.h"
#include "esp_heap_caps.h"

/* External Semaphores */
extern SemaphoreHandle_t semIndEntry;
extern SemaphoreHandle_t semWifiEntry;
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

    int8_t oneSecCounter = 7;

    while (true)
    {
        switch (sysOP)
        {
        case SYS_OP::Run: // We would like to achieve about a 4Hz entry cadence in the Run state.
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
                    if (show & _showRun)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_NOTIFY::NONE");
                    break;
                }

                case SYS_NOTIFY::WIFI_CONNECTED:
                {
                    if (show & _showRun)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_NOTIFY::WIFI_CONNECTED"); // Tell all parties who care that Internet is available.

                    sysWifiConnState = WIFI_CONN_STATE::WIFI_CONNECTED_STA;
                    break;
                }

                case SYS_NOTIFY::WIFI_CONNECTING:
                {
                    if (show & _showRun)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_NOTIFY::WIFI_CONNECTING"); // Tell all parties who care that Internet is available.

                    sysWifiConnState = WIFI_CONN_STATE::WIFI_CONNECTING_STA;
                    break;
                }

                case SYS_NOTIFY::WIFI_DISCONNECTING:
                {
                    if (show & _showRun)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_NOTIFY::WIFI_DISCONNECTING"); // Tell all parties who care that the Internet is not avaiable.
                    break;
                }

                case SYS_NOTIFY::WIFI_DISCONNECTED:
                {
                    if (show & _showRun)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_NOTIFY::WIFI_DISCONNECTED"); // Wifi is competlely ready to be connected again.

                    sysWifiConnState = WIFI_CONN_STATE::WIFI_DISCONNECTED;
                    break;
                }

                case SYS_NOTIFY::WIFI_SHUTDOWN:
                {
                    if (show & _showRun)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_NOTIFY::WIFI_SHUTDOWN");
                    // System must now call the Wifi's destructor explicitly.
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
                    if (show & _showPayload)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): Payload = " + strCmdPayload);
                }

                switch (ptrSYSCmdRequest->requestedCmd)
                {
                case SYS_COMMAND::NONE:
                {
                    if (show & _showRun)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_COMMAND::NONE");
                    break;
                }
                }

                xQueueReceive(systemCmdRequestQue, (void *)&ptrSYSCmdRequest, pdMS_TO_TICKS(10));
            }

            /* Pending actions */
            if (lockGetBool(&saveToNVSFlag))
            {
                lockSetBool(&saveToNVSFlag, false);
                saveVariablesToNVS();
            }

            if (lockGetUint8(&diagSys)) // We may run periodic or commanded diagnostics
            {
                uint8_t diagSysValue = lockGetUint8(&diagSys);

                if (diagSysValue & _diagHeapCheck)
                {
                    lockAndUint8(&diagSys, _diagHeapCheck); // Clear the bit
                    heap_caps_check_integrity_all(true);    // Esp library test
                }
                else if (diagSysValue & _printRunTimeStats)
                {
                    lockAndUint8(&diagSys, _printRunTimeStats); // Clear the bit
                    printRunTimeStats();                        // This diagnostic will affect your process over a 45 seconds period.  Can't use without special Menuconfig settings set.
                }
                else if (diagSysValue & _printRunTimeStats)
                {
                    lockAndUint8(&diagSys, _printMemoryStats); // Clear the bit
                    printMemoryStats();
                }
                else if (diagSysValue & _printRunTimeStats)
                {
                    lockAndUint8(&diagSys, _printTaskInfo); // Clear the bit
                    printTaskInfo();
                }
            }

            if (--oneSecCounter < 1) // Low accuracy counter based on normal delays inside this run function.
            {
                oneSecCounter = 7; // Can be adjusted up or down to yield are more pleasant result.
                //
                // If Unconnected to a Host - flash Red
                // If Connecting  to a Host - flash Green
                // If Connected   to a Host - flash Blue
                //
                int32_t val = 0;
                if (sysWifiConnState == WIFI_CONN_STATE::WIFI_DISCONNECTED)
                    val = 0x11000209;
                if (sysWifiConnState == WIFI_CONN_STATE::WIFI_CONNECTING_STA)
                    val = 0x21000209;
                else if (sysWifiConnState == WIFI_CONN_STATE::WIFI_CONNECTED_STA)
                    val = 0x41000209;

                if (val > 0)
                {
                    if (queHandleIndCmdRequest != nullptr)
                        xQueueSendToBack(queHandleIndCmdRequest, (void *)&val, pdMS_TO_TICKS(0));
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

                /* IOT Request and Responses */
                systemCmdRequestQue = xQueueCreate(1, sizeof(SYS_CmdRequest *)); // We hold the queue for incoming requests
                ptrSYSCmdRequest = new SYS_CmdRequest();                         // <-- Incoming request
                ptrSYSResponse = new SYS_Response();                             // --> Outgoing responses
                ptrSYSResponse->jsonResponse = nullptr;

                initSysStep = SYS_INIT::Start_Network_Interface;
                break;
            }

            case SYS_INIT::Start_Network_Interface:
            {
                if (show & _showInit)
                    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_INIT::Start_Network_Interface - Step " + std::to_string((int)SYS_INIT::Start_Network_Interface));

                ESP_GOTO_ON_ERROR(esp_netif_init(), sys_Start_Network_Interface_err, TAG, "esp_netif_init() failure."); // Network Interface initialization - starts up the TCP/IP stack.
                initSysStep = SYS_INIT::Create_Default_Event_Loop;
                break;

            sys_Start_Network_Interface_err:
                errMsg = std::string(__func__) + "(): SYS_INIT::Start_Network_Interface: error: " + esp_err_to_name(ret);
                initSysStep = SYS_INIT::Error;
                break;
            }

            case SYS_INIT::Create_Default_Event_Loop:
            {
                if (show & _showInit)
                    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_INIT::Create_Default_Event_Loop - Step " + std::to_string((int)SYS_INIT::Create_Default_Event_Loop));

                ESP_GOTO_ON_ERROR(esp_event_loop_create_default(), sys_Create_Default_Event_Loop_err, TAG, "esp_event_loop_create_default() failure.");
                initSysStep = SYS_INIT::Start_GPIO;
                break;

            sys_Create_Default_Event_Loop_err:
                errMsg = std::string(__func__) + "(): SYS_INIT::Create_Default_Event_Loop: error: " + esp_err_to_name(ret);
                initSysStep = SYS_INIT::Error;
                break;
            }

            case SYS_INIT::Start_GPIO:
            {
                if (show & _showInit)
                    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_INIT::Start_GPIO - Step " + std::to_string((int)SYS_INIT::Start_GPIO));

                initGPIOPins(); // Set up all our pin General Purpose Input Output pin definitions
                initGPIOTask(); // Assigning ISRs to pins and start GPIO Task

                // NOTE: Timer task will be not be started until System initialization is complete.
                initSysStep = SYS_INIT::Create_Indication;
                break;
            }

            case SYS_INIT::Create_Indication:
            {
                if (show & _showInit)
                    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_INIT::Create_Indication - Step " + std::to_string((int)SYS_INIT::Create_Indication));

                if (ind == nullptr)
                    ind = new Indication((uint8_t)APP_VERSION_MAJOR, (uint8_t)APP_VERSION_MINOR, (uint8_t)APP_VERSION_PATCH);

                if (ind != nullptr)
                {
                    if (show & _showInit)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_INIT::Wait_On_Indication - Step " + std::to_string((int)SYS_INIT::Wait_On_Indication));

                    initSysStep = SYS_INIT::Wait_On_Indication;
                }
                [[fallthrough]];
            }

            case SYS_INIT::Wait_On_Indication:
            {
                if (xSemaphoreTake(semIndEntry, 100) == pdTRUE)
                {
                    taskHandleIndRun = ind->getRunTaskHandle();
                    queHandleIndCmdRequest = ind->getCmdRequestQueue();
                    xSemaphoreGive(semIndEntry);
                    initSysStep = SYS_INIT::Create_Wifi;
                }
                break;
            }

            case SYS_INIT::Create_Wifi:
            {
                if (show & _showInit)
                    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_INIT::Create_Wifi - Step " + std::to_string((int)SYS_INIT::Create_Wifi));

                if (wifi == nullptr)
                    wifi = new Wifi();

                if (wifi != nullptr)
                {
                    if (show & _showInit)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_INIT::Wait_On_Wifi - Step " + std::to_string((int)SYS_INIT::Wait_On_Wifi));

                    initSysStep = SYS_INIT::Wait_On_Wifi;
                }
                [[fallthrough]];
            }

            case SYS_INIT::Wait_On_Wifi:
            {
                if (xSemaphoreTake(semWifiEntry, 100) == pdTRUE)
                {
                    taskHandleWIFIRun = wifi->getRunTaskHandle();
                    queHandleWIFICmdRequest = wifi->getCmdRequestQueue();
                    xSemaphoreGive(semWifiEntry);
                    initSysStep = SYS_INIT::Finished;
                }
                break;
            }

            case SYS_INIT::Finished:
            {
                if (show & _showInit)
                    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_INIT::Finished");

                bootCount++;
                lockSetUint8(&saveToNVSDelaySecs, 2);

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
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + errMsg);
            sysOP = SYS_OP::Idle;
            break;
        }

        case SYS_OP::Idle:
        {
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): Idle...");
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