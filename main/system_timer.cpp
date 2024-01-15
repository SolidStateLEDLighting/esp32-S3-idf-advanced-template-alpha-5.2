#include "system_.hpp"

#include "esp_system.h"
#include "esp_heap_caps.h"

/* External Variables */
extern bool blnallowSwitchGPIOinput;
extern uint8_t SwitchDebounceCounter;

// Local Variables
uint8_t halfSecond = 50;
uint8_t OneSecond = 2;
uint8_t FiveSeconds = 5;
uint8_t TenSeconds = 10;
uint8_t OneMinute = 6;
uint8_t FiveMinutes = 5;

//
// The timer here is of good precision but because all control to other objects is done through freeRTOS mechanisms, they would not
// be best for short time intervals.  So, we refrain from calling other objects for short time periods.
//
void System::initGenTimer(void)
{
    xTaskCreate(runGenTimerTaskMarshaller, "sys_tmr", 1024 * timerStackSizeK, this, 8, &taskHandleRunSystemTimer);

    const esp_timer_create_args_t general_timer_args = {
        &System::genTimerCallback,
        this,
        ESP_TIMER_TASK,
        "general_timer", // name is optional, but may help identify the timer when debugging
        true,
    };

    esp_err_t ret = ESP_OK;
    ESP_GOTO_ON_ERROR(esp_timer_create(&general_timer_args, &handleGenTimer), sys_initGenTimer_err, TAG, "esp_timer_create() failed");
    ESP_GOTO_ON_ERROR(esp_timer_start_periodic(handleGenTimer, TIMER_PERIOD_100Hz), sys_initGenTimer_err, TAG, "esp_timer_create() failed");
    return;

sys_initGenTimer_err:
    errMsg = std::string(__func__) + "(): " + esp_err_to_name(ret);
    sysOP = SYS_OP::Error;
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
            // Important Note:  We are breaking the rules here by accessing variables in 2 different tasks without locking them.
            // In this particular case any errors that would result can not be seen.  We might have a skipped count or a
            // slightly longer delay.  The error would not matter.
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

/* Periodic Actions */
void System::halfSecondActions(void)
{
    if (showSys & _showTimerSeconds)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): Half Second");
}

void System::oneSecondActions(void)
{
    if (showSys & _showTimerSeconds)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): One Second");
    //
    // When we are working with multiple variables at the same time, we don't want 'save to NVS' being called too quickly.
    // Allow a save even if we are in the process of reboot count-down.
    //
    if (saveToNVSDelaySecs > 0)
    {
        if (--saveToNVSDelaySecs < 1)
            saveToNVSFlag = true;
    }

    /* Reboot Request */
    if (rebootTimerSec > 0)
    {
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): Reboot in " + std::to_string(rebootTimerSec));
        if (--rebootTimerSec < 1)
            esp_restart(); // Reboot
        else
            return; // Don't permit further entry into the oneSecondActions routine while waiting for the reboot to begin.
    }

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

    if (queHandleIndCmdRequest != nullptr)
        xQueueSendToBack(queHandleIndCmdRequest, (void *)&val, pdMS_TO_TICKS(0));
}

void System::fiveSecondActions(void)
{
    if (showSys & _showTimerSeconds)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): Five Seconds");

    // int32_t val = 0x41000209; // 5 second heartbeat in blue

    // if (queHandleIndCmdRequest != nullptr)
    //     xQueueSendToBack(queHandleIndCmdRequest, (void *)&val, pdMS_TO_TICKS(0));
}

void System::tenSecondActions(void)
{
    if (showSys & _showTimerSeconds)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): Ten Seconds");

    lockOrUint8(&diagSys, _diagHeapCheck); // Set the diag bit to run the heap_caps_check_integrity_all(true) test
}

void System::oneMinuteActions(void)
{
    if (showSys & _showTimerMinutes)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): One Minute");
}

void System::fiveMinuteActions(void)
{
    if (showSys & _showTimerMinutes)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): Five Minutes");
}
