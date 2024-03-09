/** YOU MUST VIEW THIS PROJECT IN VS CODE TO SEE FOLDING AND THE PERFECTION OF THE DESIGN **/

#include "system_.hpp"

#include "esp_netif.h"
#include "esp_check.h"
#include "esp_heap_caps.h"

/* External Semaphores */
extern SemaphoreHandle_t semIndEntry;
extern SemaphoreHandle_t semWifiEntry;
extern SemaphoreHandle_t semSysRouteLock;
extern SemaphoreHandle_t semSysUint8Lock;
extern SemaphoreHandle_t semSysIndLock; // Local Indication Lock

void System::runMarshaller(void *arg)
{
    ((System *)arg)->run();
    vTaskDelete(NULL);
}

void System::run(void)
{
    esp_err_t ret = ESP_OK;
    int8_t oneSecCounter = 7;

    while (true)
    {
        switch (sysOP)
        {
        case SYS_OP::Run: // We would like to achieve about a 4Hz entry cadence in the Run state.
        {
            /*  Service all Task Notifications */
            /* Task Notifications should be used for notifications or commands which need no input and return no data. */
            sysTaskNotifyValue = static_cast<SYS_NOTIFY>(ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(250))); // Wait 250 mSec for any notifications then move on.

            if (sysTaskNotifyValue > static_cast<SYS_NOTIFY>(0)) // We are not using commands right now, so there is no value is waiting here.
            {
                switch (sysTaskNotifyValue)
                {
                case SYS_NOTIFY::NFY_WIFI_CONNECTING:
                {
                    if (show & _showRun)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_NOTIFY::NFY_WIFI_CONNECTING"); // Tell all parties who care that Internet is available.
                    sysWifiConnState = WIFI_CONN_STATE::WIFI_CONNECTING_STA;
                    break;
                }

                case SYS_NOTIFY::NFY_WIFI_CONNECTED:
                {
                    if (show & _showRun)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_NOTIFY::NFY_WIFI_CONNECTED"); // Tell all parties who care that Internet is available.
                    sysWifiConnState = WIFI_CONN_STATE::WIFI_CONNECTED_STA;
                    break;
                }

                case SYS_NOTIFY::NFY_WIFI_DISCONNECTING:
                {
                    if (show & _showRun)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_NOTIFY::NFY_WIFI_DISCONNECTING"); // Tell all parties who care that the Internet is not avaiable.
                    sysWifiConnState = WIFI_CONN_STATE::WIFI_DISCONNECTING_STA;
                    break;
                }

                case SYS_NOTIFY::NFY_WIFI_DISCONNECTED:
                {
                    if (show & _showRun)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_NOTIFY::NFY_WIFI_DISCONNECTED"); // Wifi is competlely ready to be connected again.
                    sysWifiConnState = WIFI_CONN_STATE::WIFI_DISCONNECTED;
                    break;
                }

                case SYS_NOTIFY::CMD_DESTROY_WIFI:
                {
                    if (show & _showRun)
                        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SYS_NOTIFY::WIFI_SHUTDOWN");

                    if (wifi != nullptr)
                    {
                        if (semWifiEntry != nullptr)
                        {
                            xSemaphoreTake(semWifiEntry, portMAX_DELAY); // Wait here until we gain the lock.

                            // Send out notifications to any object that uses the wifi and tell them wifi is no longer available.

                            taskHandleWIFIRun = nullptr;       // Clear the wifi task handle
                            queHandleWIFICmdRequest = nullptr; // Clear the wifi Command Queue handle

                            delete wifi;    // Locking the object will be done inside the destructor.
                            wifi = nullptr; // Destructor will not set pointer null.  We must to do that manually.

                            // Note: The semWifiEntry semaphore is already destroyed - so don't "Give" it or a run time error will occur

                            if (show & _showRun)
                                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): wifi deleted");
                        }
                    }
                    break;
                }

                case SYS_NOTIFY::CMD_DESTROY_INDICATION:
                {
                    if (semIndEntry != nullptr)
                    {
                        xSemaphoreTake(semIndEntry, portMAX_DELAY); // Wait here until we gain the lock.

                        if (ind != nullptr) // Make sure we already have a indication object
                        {
                            // Send out notifications to any object that uses indication -- and tell them indication is no longer available.

                            if (xSemaphoreTake(semSysIndLock, portMAX_DELAY)) // Several tasks can access Indication varaibles.
                            {
                                taskHandleIndRun = nullptr;       // Clear the indication task handle
                                queHandleIndCmdRequest = nullptr; // Clear the indication Command Queue handle
                                xSemaphoreGive(semSysIndLock);
                            }

                            delete ind; // Locking the object will be done inside the destructor.

                            if (ind != nullptr)
                                ind = nullptr; // Destructor will not set pointer null.  We do that manually.

                            if (show & _showRun)
                                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): indication deleted");
                        }
                    }
                }
                }
            }

            /* Service all Incoming Commands */
            /* Queue based commands should be used for commands which may provide input or perhaps return data. */
            if (xQueuePeek(systemCmdRequestQue, (void *)&ptrSYSCmdRequest, 0)) // We cycle through here and look for incoming mail box command requests
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

            /* Pending Actions and State Change Actions */
            if (lockGetBool(&saveToNVSFlag))
            {
                lockSetBool(&saveToNVSFlag, false);
                saveVariablesToNVS();
            }

            if (lockGetUint8(&diagSys)) // We may run periodic or commanded diagnostics
                runDiagnostics();

            //
            // Temporary helper - RGB visual indicator is see if we are disconnected, in process, or connected to a Wifi host
            // This area is not included in documentation because it will be removed.
            //
            if (--oneSecCounter < 1) // Low accuracy counter based on normal delays inside this run function.
            {
                // ESP_LOGW(TAG, "sysWifiConnState is %d", (int)sysWifiConnState); // Testing and debugging
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
                    if (xSemaphoreTake(semSysIndLock, portMAX_DELAY))
                    {
                        if (queHandleIndCmdRequest != nullptr)
                            xQueueSendToBack(queHandleIndCmdRequest, (void *)&val, pdMS_TO_TICKS(0));
                        xSemaphoreGive(semSysIndLock);
                    }
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

                initSysStep = SYS_INIT::Start_Network_Interface;
                [[fallthrough]];
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
                if (xSemaphoreTake(semIndEntry, 100))
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
                if (xSemaphoreTake(semWifiEntry, 100))
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

                initSysTimerTask(); // Starting System Timer task
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
            vTaskDelay(pdMS_TO_TICKS(5000));
            break;
        }
        }
        taskYIELD();
    }
}