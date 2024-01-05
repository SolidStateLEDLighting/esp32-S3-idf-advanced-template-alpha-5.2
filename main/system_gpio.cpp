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

bool blnallowSwitchGPIOinput = true; // These variables are used for switch input debouncing
uint8_t SwitchDebounceCounter = 0;

QueueHandle_t xQueueGPIOEvents = nullptr;

void System::initGPIOPins(void) // We initial all pins possible here.
{
    //
    // At one time, we were initializing all pins here, but new libraries have been taking some of that job away.
    //

    //
    // Tactile Switch(es)
    //
    gpio_config_t gpioSW1; // GPIO_BOOT_SW
    gpioSW1.pin_bit_mask = 1LL << SW1;
    gpioSW1.mode = GPIO_MODE_INPUT;
    gpioSW1.pull_up_en = GPIO_PULLUP_ENABLE;
    gpioSW1.intr_type = GPIO_INTR_NEGEDGE;
    gpio_config(&gpioSW1);
}

//
// This ISR is set apart because tactile switch input needs to be handled with a debouncing algorithm.
//
void IRAM_ATTR GPIOSwitchIsrHandler(void *arg)
{
    if (blnallowSwitchGPIOinput)
    {
        xQueueSendToBackFromISR(xQueueGPIOEvents, &arg, NULL);
        SwitchDebounceCounter = 50; // Reject all input for 5/10 of a second -- counter is running in system_timer
        blnallowSwitchGPIOinput = false;
    }
}

//
// Normal (non-switch) GPIO ISR handling would warrant no debouncing delay.
//
void IRAM_ATTR GPIOIsrHandler(void *arg)
{
    xQueueSendToBackFromISR(xQueueGPIOEvents, &arg, NULL);
}

void System::initGPIOTask(void)
{
    if (show & _showInit)
        ESP_LOGI(TAG, "%s()", __func__); // Our Error handlers and loggers are not running yet.

    esp_err_t ret = ESP_OK;

    xQueueGPIOEvents = xQueueCreate(1, sizeof(uint32_t)); // Create a queue to handle gpio events from isr
    ESP_GOTO_ON_FALSE(xQueueGPIOEvents, ESP_FAIL, sys_GPIOIsrHandler_err, TAG, "xQueueCreate() failed");

    ESP_GOTO_ON_ERROR(gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT), sys_GPIOIsrHandler_err, TAG, "() failed");

    if (show & _showInit)
        ESP_LOGI(TAG, "%s(): Started gpio isr service...", __func__);

    ESP_GOTO_ON_ERROR(gpio_isr_handler_add(SW1, GPIOSwitchIsrHandler, (void *)SW1), sys_GPIOIsrHandler_err, TAG, "() failed");

    SwitchDebounceCounter = 30;
    blnallowSwitchGPIOinput = true;

    xTaskCreate(runGPIOTaskMarshaller, "sys_gpio", 1024 * gpioStackSizeK, this, 7, &runTaskHandleSystemGPIO); // (1) Low number indicates low priority task
    return;

sys_GPIOIsrHandler_err:
    ESP_LOGI(TAG, "%s(): error %s", __func__, esp_err_to_name(ret)); // Our Error handlers and loggers are not running yet.
}

void System::runGPIOTaskMarshaller(void *arg) // This function can be resolved at run time by the compiler.
{
    ((System *)arg)->runGPIOTask();
    vTaskDelete(NULL);
}

void System::runGPIOTask(void)
{
    uint32_t io_num;

    while (true)
    {
        if (xQueueReceive(xQueueGPIOEvents, (void *)&io_num, portMAX_DELAY)) // There is never any reason to yield.
        {
            // ESP_LOGI(TAG, "xQueueGPIOEvents   io_num  %d", io_num);
            // ESP_LOGI(TAG, "xQueueGPIOEvents ...sysOP = %x", (uint8_t)sysOP);
            if (sysOP == SYS_OP::Init) // If we haven't finished out our initialization -- discard items in our queue.
                continue;

            switch (io_num)
            {
            case SW1: // Call Test fuctions here
            {
                // if (taskHandleIndicationRun != nullptr)
                // {
                //     uint32_t Notification = ((int32_t)IND_NOTIFY::SET_C_COLOR_DEFAULT | 0x00000025);
                //     if(xTaskNotify(taskHandleIndicationRun, static_cast<uint32_t>(Notification), eSetValueWithoutOverwrite))
                // vTaskDelay(pdMS_TO_TICKS(10));
                // }
                // else
                //     ESP_LOGE(TAG, "taskHandleIndicationRun is null... We can't send a task notification to Indication");

                // NVS
                // char nameWifi[] = "wifi";
                // eraseNVSPartition();         // Erases all the partition variables - defaults to "nvs"
                // eraseNVSNamespace("system"); // Erases only System variables
                // eraseNVSNamespace(nameWifi); // Erases only Wifi variables
                break;
            }

            default:
                ESP_LOGI(TAG, "Missing Case for io_num  %ld...(runGPIOTask)", io_num);
                break;
            }
        }
    }
}