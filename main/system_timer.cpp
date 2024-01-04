#include "system_.hpp"

#include "esp_system.h"
#include "esp_heap_caps.h"

extern bool blnallowSwitchGPIOinput;
extern uint8_t SwitchDebounceCounter;

auto halfSecond = 50;
auto OneSecond = 2;
auto FiveSeconds = 5;
auto TenSeconds = 10;
auto OneMinute = 6;
auto FiveMinutes = 5;

void System::initGenTimer(void)
{
    xTaskCreate(runGenTimerTaskMarshaller, "sys_tmr", 1024 * 4, this, 8, &taskHandleRunSystemTimer);

    const esp_timer_create_args_t general_timer_args = {
        &System::genTimerCallback,
        this,
        ESP_TIMER_TASK,
        "general_timer", // name is optional, but may help identify the timer when debugging
        true,
    };

    esp_err_t ret = esp_timer_create(&general_timer_args, &handleGenTimer);

    if unlikely (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "%s::esp_timer_create failed.  err = 0x%X", __func__, ret);
        return;
    }

    ret = esp_timer_start_periodic(handleGenTimer, TIMER_PERIOD_100Hz);

    if unlikely (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "%s::esp_timer_start_periodic failed.  err = 0x%X", __func__, ret);
        return;
    }
}

void IRAM_ATTR System::genTimerCallback(void *arg)
{
    // Our priorty here is to exit our ISR with minumum processing so the system can return to normal task servicing quickly.
    // Therefore, we will exercise Deferred Interrupt Processing as often as posible.
    // NOTE: Any high priorty task will essentially run as if it were the ISR itself if there are no other equally high prioirty tasks running.
    //
    vTaskNotifyGiveFromISR(((System *)arg)->taskHandleRunSystemTimer, NULL);
}

void System::runGenTimerTaskMarshaller(void *arg)
{
    ((System *)arg)->runGenTimerTask();
    vTaskDelete(NULL);
}

void System::runGenTimerTask(void)
{
    while (true)
    {
        //
        // 100hz Processing
        //
        // We are using a generic task notification to trigger this routine at 100hz.  This command blocks until a
        // notification arrives.  We don't examine any notification value.  The value is ignored.
        //
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (SwitchDebounceCounter > 0) // Perform GPIO Switch Debouncing delay here.
        {
            if (--SwitchDebounceCounter < 1)
                blnallowSwitchGPIOinput = true;
        }

        if (halfSecond > 0)
        {
            if (--halfSecond < 1)
            {
                halfSecondActions();

                if (OneSecond > 0)
                {
                    if (--OneSecond < 1)
                    {
                        oneSecondActions();

                        if (FiveSeconds > 0)
                        {
                            if (--FiveSeconds < 1)
                            {
                                fiveSecondActions();
                                FiveSeconds = 5;
                            }
                        }

                        if (TenSeconds > 0)
                        {
                            if (--TenSeconds < 1)
                            {
                                tenSecondActions();
                                TenSeconds = 10;
                            }
                        }

                        if (OneMinute > 0)
                        {
                            if (--OneMinute < 1)
                            {
                                oneMinuteActions();

                                if (FiveMinutes > 0)
                                {
                                    if (--FiveMinutes < 1)
                                    {
                                        fiveMinuteActions();
                                        FiveMinutes = 5;
                                    }
                                }
                                OneMinute = 60;
                            }
                        }
                        OneSecond = 2;
                    }
                }
                halfSecond = 50;
            }
        }
    }
}

/* Periodic One Second Actions */
void System::halfSecondActions(void)
{
    if (showSys & _showTimerSeconds)
        ESP_LOGI(TAG, "Half Second");
    //
    // Right now, we will allow the half second action to be completely filled with one objective -- to handle cat detection and image acquisition.
    //

    // First -- if we are not connected, our entire state model has to be cancelled.
}

void System::oneSecondActions(void)
{
    if (showSys & _showTimerSeconds)
        ESP_LOGI(TAG, "One Second");

    //
    // When we are working with multiple variables at the same time, we don't want 'save to NVS' being called too quickly.
    // Allow a save even if we are in the process of reboot counte-down.
    //
    if (saveToNVSDelayCount > 0)
    {
        if (--saveToNVSDelayCount < 1)
            saveToNVSFlag = true;
    }

    if (rebootTimerSec > 0) // Reboot Timer
    {
        ESP_LOGW(TAG, "Reboot in %d", rebootTimerSec);
        if (--rebootTimerSec < 1)
            esp_restart(); // Reboot
        else
            return; // Don't permit further entry into the oneSecondActions routine while waiting for the reboot to begin.
    }

    //
    //  Ask Wifi To Connect based on a time delay.  This reduces excessive attempts in a short period of time.
    //
    // if (startWifiDelaySecondsCountDown > 0)
    // {
    //     if (--startWifiDelaySecondsCountDown < 1)
    //     {
    //         if ((wifi != nullptr) && (currWifiState != SYS_WIFI_STATE::CONNECTED)) // First look to see if we are connected already...
    //         {
    //             ESP_LOGI(TAG, "Asking Wifi to Connect...");

    //             if (taskHandleWIFIRun != nullptr) // Inform Wifi so it may choose to connect
    //             {
    //                 if (xTaskNotify(taskHandleWIFIRun, static_cast<uint8_t>(WIFI_NOTIFY::CMD_CONNECT), eSetValueWithoutOverwrite))
    //                     vTaskDelay(pdMS_TO_TICKS(10));
    //             }
    //             else
    //                 ESP_LOGE(TAG, "taskHandleWIFIRun is null... We can't send CMD_CONNECT task notifications to Wifi");
    //         }
    //     }
    // }

    // if (currWifiState < SYS_WIFI_STATE::CONNECTED) // Anytime we are less than connected
    // {
    //     if (wifiSSIDPasswordOk == false) // wifiSSIDPasswordOk == true indicates that we have confirmed the SSID/Password currently good
    //     {
    //     }
    //     else // We have logged into the Wifi router, but don't have an IP Address yet.
    //     {
    //         //
    //         // There are circumstances where our Wifi connection progress will stall before and IP address is assigned.
    //         // We catch those occurances here and restart the Wifi object if necessary.  Additionally, we record every wifi object restart in NVS.
    //         //
    //         // lookingForIPAddressCounter++;

    //         if (lookingForIPAddressCounter > 30) // Watch the count between 30 and 45 seconds...
    //             ESP_LOGW(TAG, "Wifi Connection waiting for an IP address running long at %d seconds.", lookingForIPAddressCounter);

    //         if (lookingForIPAddressCounter > 45)
    //         {
    //             ESP_LOGW(TAG, "Wifi Connect Timed Out.  Restarting Wifi...");
    //             wifiRestrtCount++; // Record this restart in NVS
    //             saveToNVSFlag = true;
    //             vTaskDelay(pdMS_TO_TICKS(50));
    //             wifiRestart = true;
    //         }
    //     }
    // }
    // else // The only other state possible is SYS_WIFI_STATE::CONNECTED
    // {
    //     lookingForSSIDandPasswordSecsCounter = 0; // Reset our counter
    // }
}

void System::fiveSecondActions(void)
{
    if (showSys & _showTimerSeconds)
        ESP_LOGI(TAG, "Five Seconds");

    // int32_t val = 0x21000209; // 5 second heartbeat in green
    // if (queHandleINDCmdRequest != nullptr)
    //     xQueueSendToBack(queHandleINDCmdRequest, (void *)&val, pdMS_TO_TICKS(0));
}

void System::tenSecondActions(void)
{
    if (showSys & _showTimerSeconds)
        ESP_LOGI(TAG, "Ten Seconds");

    lockOrUint8(&diagSys, _diagHeapCheck); // Set the diag bit to run the heap_caps_check_integrity_all(true) test
}

void System::oneMinuteActions(void)
{
    if (showSys & _showTimerMinutes)
        ESP_LOGI(TAG, "One Minute");
}

void System::fiveMinuteActions(void)
{
    if (showSys & _showTimerMinutes)
        ESP_LOGI(TAG, "Five Minutes");

    // if (blnSNTPTimeValid)
    // printTime();
}
