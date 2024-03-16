#include "system_.hpp"

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_check.h"

#include "esp_pm.h"

#include "esp_sleep.h"

/* External Semaphores */
extern SemaphoreHandle_t semNVSEntry;
extern SemaphoreHandle_t semWifiEntry;
extern SemaphoreHandle_t semIndEntry;

//
// This source file is all about running tests.  All functions here are called only from our runGPIOTask() function.
//
// These will be grouped in these categories:
// 1) Object Lifecycle
// 2) Sleep Modes
// 3) NVS
// 4) Indication
// 5) WIFI
// 6)

//
// Object Lifecycle
//
void System::test_objectLifecycle_create(SYS_TEST_TYPE *type, uint8_t *index)
{
    switch (*index)
    {
    case 0:
    {
        if (wifi == nullptr)
            wifi = new Wifi();

        if (wifi != nullptr) // Make sure memory was allocated
        {
            if (xSemaphoreTake(semWifiEntry, 100)) // Get a lock on the object after it initializes
            {
                taskHandleWIFIRun = wifi->getRunTaskHandle();
                queHandleWIFICmdRequest = wifi->getCmdRequestQueue();
                xSemaphoreGive(semWifiEntry); // Release lock

                // Send out notifications to any object that needs the wifi and tell them wifi is now available.

                ESP_LOGW(TAG, "wifi instantiated");
            }
        }
        *type = SYS_TEST_TYPE::LIFE_CYCLE_DESTROY;
        *index = 0;
        break;
    }

    case 1:
    {
        break;
    }

    case 2:
    {
        break;
    }
    }
}

void System::test_objectLifecycle_destroy(SYS_TEST_TYPE *type, uint8_t *index)
{
    switch (*index)
    {
    case 0:
    {
        if (wifi != nullptr)
        {
            if (semWifiEntry != nullptr)
            {
                xSemaphoreTake(semWifiEntry, portMAX_DELAY); // Wait here until we gain the lock.

                // Send out notifications to any object that uses the wifi and tell them wifi is no longer available.

                taskHandleWIFIRun = nullptr;       // Reset the wifi handles
                queHandleWIFICmdRequest = nullptr; //

                delete wifi;                   // Lock on the object will be done inside the destructor.
                wifi = nullptr;                // Destructor will not set pointer null.  We have to do that manually.
                ESP_LOGW(TAG, "wifi deleted"); //

                // Note: The semWifiEntry semaphore is already destroyed - so don't "Give" it or a run time error will occur
            }
        }
        *type = SYS_TEST_TYPE::LIFE_CYCLE_CREATE;
        *index = 0;
        return;
        break;
    }

    case 1:
    {
        break;
    }

    case 2:
    {
        break;
    }
    }

    if (++*index > 0) // We set the limit based on our test sequence
    {
        ESP_LOGW("", "Restarting text index...");
        *index = 0;
    }
}

//
// Low Power and Sleep Modes
//
void System::test_power_management(SYS_TEST_TYPE *type, uint8_t *index)
{
    switch (*index)
    {
    case 0:
    {
        // Use of Power Management creates interrupt latency because the system has to resume power consuming actions.  This takes time.
        // If you always need a minimum response time then you should not use Power Management features.

        // # Enable support for power management
        // CONFIG_PM_ENABLE=y                   // [X] Support for power management
        // CONFIG_FREERTOS_USE_TICKLESS_IDLE=y  // [X] configUSE_TICKLESS_IDLE

        // # Put related source code in IRAM
        // CONFIG_PM_SLP_IRAM_OPT=y  // [X] Put lightsleep related codes in internal RAM
        // CONFIG_PM_RTOS_IDLE_OPT=y // [X] Put RTOS IDLE related codes in internal RAM

        // # Enable wifi sleep iram optimization
        // CONFIG_ESP_WIFI_SLP_IRAM_OPT=y // [X] WiFi SLP IRAM speed optimization

        ESP_LOGW(TAG, "Activating Power Management...");
        ESP_ERROR_CHECK(uart_wait_tx_idle_polling((uart_port_t)CONFIG_ESP_CONSOLE_UART_NUM));

        esp_pm_config_t pm_config = {
            .max_freq_mhz = 240,        // These values can be set anywhere we set them manually here.
            .min_freq_mhz = 80,         // 
            .light_sleep_enable = true, // Automatic light sleep is enabled if tickless idle support is enabled.
        };

        ESP_ERROR_CHECK(esp_pm_configure(&pm_config));

        *type = SYS_TEST_TYPE::WIFI;
        *index = 0;
        break;
    }

    case 1:
    {
        break;
    }

    case 2:
    {
        break;
    }
    }

}


void System::test_low_power_sleep(SYS_TEST_TYPE *type, uint8_t *index)
{
    switch (*index)
    {
    case 0: // Enter Light Sleep (Full RAM Retention)
    {
        while (gpio_get_level(SW1) == 0)   // This is required to allow time for the circuit to rise after the interrupt trigger (low).
            vTaskDelay(pdMS_TO_TICKS(10)); // which brought the call here.  We need to see a high before we can continue.  This takes time.

        ESP_ERROR_CHECK(gpio_wakeup_enable(SW1, GPIO_INTR_LOW_LEVEL)); // Our GPIO0 is already configured for input active LOW
        ESP_ERROR_CHECK(esp_sleep_enable_gpio_wakeup());

        // WARNING: You will need to be sure that you are attached to the correct Console UART or you may not see the serial output
        //          resume when you awake from sleep.   Select the serial port output that is native to the Tx/Rx pins - not the on-chip USB output.

        ESP_LOGW(TAG, "Entering Light Sleep...");
        ESP_ERROR_CHECK(uart_wait_tx_idle_polling((uart_port_t)CONFIG_ESP_CONSOLE_UART_NUM)); // This call flushes the serial port FIFO buffer before sleep

        // By default, esp_deep_sleep_start() and esp_light_sleep_start() functions will power down all RTC power domains which are not
        // needed by the enabled wakeup sources.  To override this behaviour, esp_sleep_pd_config() function is provided.
        // All examples here are the default where they are powered off if not needed (AUTO).
        // You may over-ride with the option ESP_PD_OPTION_ON where that domains remains on during sleep.
        ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO));
        ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL, ESP_PD_OPTION_AUTO));
        ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_RC_FAST, ESP_PD_OPTION_AUTO));
        ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_CPU, ESP_PD_OPTION_AUTO));
        ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_VDDSDIO, ESP_PD_OPTION_AUTO));
        ESP_ERROR_CHECK(esp_sleep_pd_config(ESP_PD_DOMAIN_MODEM, ESP_PD_OPTION_AUTO));

        esp_light_sleep_start();
        //
        // The active interrupt on this GPIO0 pin doesn't seem to have an effect on our wake-up input
        // which occurs right here...  I'm guessing that this interrupt doesn't get registered in light sleep
        // or it is lost/ignored in the wake-up process.
        //
        ESP_LOGW(TAG, "Returned from Light Sleep...");
        ESP_ERROR_CHECK(gpio_wakeup_disable(SW1));

        *type = SYS_TEST_TYPE::WIFI;
        *index = 0;
        break;
    }

    case 1: // Enter Deep Sleep (RAM is lost - RTC Memory holds data)
    {
        break;
    }

    case 2: // Enter Hibernation Mode (no RAM data can be saved here)
    {
        break;
    }
    }

    // CHANGE YOUR TEST AREA AND INDEX AS NEEDED FOR THE NEXT SEQUENCE YOU WANT
}

//
// NVS
//
void System::test_nvs(SYS_TEST_TYPE *type, uint8_t *index)
{
    // bool testBool = false;
    // std::string testString = "";
    // std::string string1 = "The quick brown fox jumps over the lazy dog";
    // std::string string2 = "Now is the time for all good men to come to the aid of their country.";
    // uint8_t testUInteger8 = 0;
    // uint32_t testUInteger32 = 0;
    // int32_t testInteger32 = 555; // Should test number going negative

    switch (*index)
    {
    case 0:
    {
        if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
        {
            ESP_LOGW(TAG, "eraseNVSPartition");
            nvs->eraseNVSPartition();
            esp_restart(); // Must do a complete reboot immediately or system will crash in any nvs instruction.  Must completely reinitialize nvs at startup.
            xSemaphoreGive(semNVSEntry);
        }
        ++*index;
        break;
    }

    case 1:
    {
        break;
    }

    case 2:
    {
        break;
    }
    }

    // CHANGE YOUR TEST AREA AND INDEX AS NEEDED FOR THE NEXT SEQUENCE YOU WANT
}

//
// Indication
//
void System::test_indication(SYS_TEST_TYPE *type, uint8_t *index)
{
    int32_t val = 0;

    switch (*index)
    {
    case 0:
    {
        val = 0x11000209; // red1 Short

        if (queHandleIndCmdRequest != nullptr)
            xQueueSendToBack(queHandleIndCmdRequest, (void *)&val, pdMS_TO_TICKS(0));

        ++*index;
        break;
    }

    case 1:
    {
        val = 0x22421219; // green2/blue2 Long

        if (queHandleIndCmdRequest != nullptr)
            xQueueSendToBack(queHandleIndCmdRequest, (void *)&val, pdMS_TO_TICKS(0));

        ++*index;
        break;
    }

    case 2:
    {
        val = 0x41411219; // blue1/blue1 Long

        if (queHandleIndCmdRequest != nullptr)
            xQueueSendToBack(queHandleIndCmdRequest, (void *)&val, pdMS_TO_TICKS(0));

        ++*index;
        break;
    }

    case 3:
    {
        val = 0x1E001219; // Turn R AUTO

        if (queHandleIndCmdRequest != nullptr)
            xQueueSendToBack(queHandleIndCmdRequest, (void *)&val, pdMS_TO_TICKS(0));

        *index = 0;
        break;
    }
    }

    // CHANGE YOUR TEST AREA AND INDEX AS NEEDED FOR THE NEXT SEQUENCE YOU WANT
}

//
// Wifi
//
void System::test_wifi(SYS_TEST_TYPE *type, uint8_t *index)
{
    switch (*index)
    {
    case 0:
    {
        while (!xTaskNotify(taskHandleWIFIRun, static_cast<uint32_t>(WIFI_NOTIFY::CMD_CONN_PRI_HOST), eSetValueWithoutOverwrite))
            vTaskDelay(pdMS_TO_TICKS(50));

        while (!xTaskNotify(taskHandleWIFIRun, static_cast<uint32_t>(WIFI_NOTIFY::CMD_RUN_DIRECTIVES), eSetValueWithoutOverwrite))
            vTaskDelay(pdMS_TO_TICKS(50));
        ++*index;
        break;
    }

    case 1:
    {
        printTaskInfo();
        ++*index;
        break;
    }

    case 2:
    {
        while (!xTaskNotify(taskHandleWIFIRun, static_cast<uint32_t>(WIFI_NOTIFY::CMD_DISC_HOST), eSetValueWithoutOverwrite))
            vTaskDelay(pdMS_TO_TICKS(50));

        while (!xTaskNotify(taskHandleWIFIRun, static_cast<uint32_t>(WIFI_NOTIFY::CMD_RUN_DIRECTIVES), eSetValueWithoutOverwrite))
            vTaskDelay(pdMS_TO_TICKS(50));
        ++*index;
        break;
    }

    case 3:
    {
        printTaskInfo();
        *index = 0;
        break;
    }
    }

    // CHANGE YOUR TEST AREA AND INDEX AS NEEDED FOR THE NEXT SEQUENCE YOU WANT
}
