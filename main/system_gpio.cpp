#include "system_.hpp"

#include "driver/gpio.h"
#include "esp_check.h"
//
// We generally handle GPIO interrupts here.  The idea is to route them to the handler which is designed for that service.
//
// We have one available tactile switch.  We can use this switch for debugging.  Notice that we debounce the
// switch in software.   We handle switch input with espressive's recommendation to first catching the ISR and then routing that
// to a queue.
//
extern SemaphoreHandle_t semNVSEntry;

bool blnallowSwitchGPIOinput = true; // These variables are used for switch input debouncing
uint8_t SwitchDebounceCounter = 0;

QueueHandle_t xQueueGPIOEvents = nullptr;

void System::initGPIOPins(void) // We initial all pins possible here.
{
    //
    // At one time, we were initializing all pins here, but new libraries have been taking some of that job away.
    //

    //
    // At may be important to disable all unused pins in a very specific ways to reduce power consumption.  This should be done here.
    //

    //
    // Tactile Switch(es)
    //
    gpio_config_t gpioSW1;             // We have one switch available in the S3 DevKitM   GPIO_BOOT_SW
    gpioSW1.pin_bit_mask = 1LL << SW1; //
    gpioSW1.mode = GPIO_MODE_INPUT;    //
    gpioSW1.pull_up_en = GPIO_PULLUP_ENABLE;
    gpioSW1.intr_type = GPIO_INTR_NEGEDGE;
    gpio_config(&gpioSW1);
}

/* This ISR is set apart because tactile switch input needs to be handled with a debouncing algorithm. */
void IRAM_ATTR GPIOSwitchIsrHandler(void *arg)
{
    if (blnallowSwitchGPIOinput)
    {
        // Important Note:  We are breaking the rules here by accessing variables in 2 different tasks without locking them.
        // In this particular case any errors that would result can not be seen.  We might have a skipped count or a
        // slightly longer delay.  The error would not matter.
        xQueueSendToBackFromISR(xQueueGPIOEvents, &arg, NULL);
        SwitchDebounceCounter = 50; // Reject all input for 5/10 of a second -- counter is running in system_timer
        blnallowSwitchGPIOinput = false;
    }
}

/* Normal GPIO ISR handling would warrant no debouncing delay. */
void IRAM_ATTR GPIOIsrHandler(void *arg)
{
    xQueueSendToBackFromISR(xQueueGPIOEvents, &arg, NULL);
}

void System::initGPIOTask(void)
{
    if (show & _showInit)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "()");

    esp_err_t ret = ESP_OK;

    xQueueGPIOEvents = xQueueCreate(1, sizeof(uint32_t)); // Create a queue to handle gpio events from isr
    ESP_GOTO_ON_FALSE(xQueueGPIOEvents, ESP_FAIL, sys_GPIOIsrHandler_err, TAG, "xQueueCreate() failed");

    ESP_GOTO_ON_ERROR(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT), sys_GPIOIsrHandler_err, TAG, "() failed");

    if (show & _showInit)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): Started gpio isr service...");

    ESP_GOTO_ON_ERROR(gpio_isr_handler_add(SW1, GPIOSwitchIsrHandler, (void *)SW1), sys_GPIOIsrHandler_err, TAG, "() failed");

    SwitchDebounceCounter = 30;
    blnallowSwitchGPIOinput = true;

    xTaskCreate(runGPIOTaskMarshaller, "sys_gpio", 1024 * gpioStackSizeK, this, 7, &runTaskHandleSystemGPIO); // (1) Low number indicates low priority task
    return;

sys_GPIOIsrHandler_err:
    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): " + std::string(esp_err_to_name(ret))); // Our Error handlers and loggers may not be running yet.
}

void System::runGPIOTaskMarshaller(void *arg) // This function can be resolved at run time by the compiler.
{
    ((System *)arg)->runGPIOTask();
    vTaskDelete(NULL);
}

void System::runGPIOTask(void)
{
    uint32_t io_num = 0;
    // int32_t val = 0;
    // int32_t brightnessLevel = 0;
    uint8_t gpioIndex = 0;

    xQueueReset(xQueueGPIOEvents);

    while (true)
    {
        if (xQueueReceive(xQueueGPIOEvents, (void *)&io_num, portMAX_DELAY)) // There is never any reason to yield.
        {
            if (sysOP == SYS_OP::Init) // If we haven't finished out our initialization -- discard items in our queue.
                continue;

            switch (io_num)
            {
            case SW1: // Call Test fuctions here
            {
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): SW1...");

                switch (gpioIndex)
                {
                case 0:
                {
                    while (!xTaskNotify(taskHandleWIFIRun, static_cast<uint32_t>(WIFI_NOTIFY::CMD_CONN_PRI_HOST), eSetValueWithoutOverwrite))
                        vTaskDelay(pdMS_TO_TICKS(10));

                    while (!xTaskNotify(taskHandleWIFIRun, static_cast<uint32_t>(WIFI_NOTIFY::CMD_RUN_DIRECTIVES), eSetValueWithoutOverwrite))
                        vTaskDelay(pdMS_TO_TICKS(10));
                    break;
                }
                case 1:
                {
                    while (!xTaskNotify(taskHandleWIFIRun, static_cast<uint32_t>(WIFI_NOTIFY::CMD_DISC_HOST), eSetValueWithoutOverwrite))
                        vTaskDelay(pdMS_TO_TICKS(10));

                    while (!xTaskNotify(taskHandleWIFIRun, static_cast<uint32_t>(WIFI_NOTIFY::CMD_RUN_DIRECTIVES), eSetValueWithoutOverwrite))
                        vTaskDelay(pdMS_TO_TICKS(10));
                    break;
                }

                case 2:
                {
                    break;
                }

                case 3:
                {
                    break;
                }

                case 4:
                {
                    break;
                }

                case 5:
                {
                    break;
                }
                }

                /*
                switch (gpioIndex) // NVS testing
                {
                case 0: // Erases all the partition variables - defaults to "nvs"
                {
                    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
                    {
                        ESP_LOGW(TAG, "eraseNVSPartition");
                        nvs->eraseNVSPartition();
                        xSemaphoreGive(semNVSEntry);
                    }
                }

                case 1: // Only erases a single nvs namespace
                {
                    std::string strNameSpace = "wifi";
                    // std::string strNameSpace = "system";
                    // std::string strNameSpace = "sntp";
                    // std::string strNameSpace = "indication";

                    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
                    {
                        ESP_LOGW(TAG, "eraseNVSNamespace %s", strNameSpace.c_str());
                        nvs->eraseNVSNamespace((char *)strNameSpace.c_str()); // Erases all the partition variables - defaults to "nvs"
                        xSemaphoreGive(semNVSEntry);
                    }
                }
                }
                */

                if (++gpioIndex > 1)
                {
                    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): gpioIndex restart...");
                    gpioIndex = 0;
                }

                break;
            }

            default:
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): Missing Case for io_num  " + std::to_string(io_num));
                break;
            }
        }
    }
}