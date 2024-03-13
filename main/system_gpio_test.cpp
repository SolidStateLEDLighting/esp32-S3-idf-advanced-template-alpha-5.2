#include "system_.hpp"

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_check.h"

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
void System::test_low_power_sleep(SYS_TEST_TYPE *type, uint8_t *index)
{
    switch (*index)
    {
    case 0: // Enter Modem Sleep
    {
        break;
    }

    case 1: // Enter Light Sleep (Full RAM Retention)
    {
        while (gpio_get_level(SW1) == 0)   // This is required to allow time for the circuit to rise from interrupt trigger (low).
            vTaskDelay(pdMS_TO_TICKS(10)); // that brought the call here.  We need to see a high before we can continue.  This takes time.

        gpio_wakeup_enable(SW1, GPIO_INTR_LOW_LEVEL); // Our GPIO0 is already configured for input active LOW
        esp_sleep_enable_gpio_wakeup();

        ESP_LOGW(TAG, "Entering Light Sleep...");
        uart_wait_tx_idle_polling((uart_port_t)CONFIG_ESP_CONSOLE_UART_NUM); // This call allows the serial port FIFO buffer to empty before sleep
        esp_light_sleep_start();
        //
        // The active GPIO interrupt on this pin doesn't seem to have an effect on our wake-up input
        // which occurs right here...  I'm guessing that this interrupt doesn't get registered in light sleep
        // or it is lost in the wake up process.
        //
        ESP_LOGW(TAG, "Returned from Light Sleep...");
        gpio_wakeup_disable(SW1);
        break;
    }

    case 2: // Enter Deep Sleep (RAM is lost - RTC Memory holds data)
    {
        break;
    }

    case 3: // Enter Hibernation Mode (no RAM data can be saved here)
    {
        break;
    }
    }

    // CHANGE YOUR TEST AREA AND INDEX AS NEEDED FOR THE NEXT SEQUENCE YOU WANT

    if (++*index > 1) // We set the limit based on our test sequence
    {
        // ESP_LOGW("", "Restarting text index...");
        *index = 1;
    }
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

    if (++*index > 0) // We set the limit based on our test sequence
    {
        ESP_LOGW("", "Restarting text index...");
        *index = 0;
    }
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
        break;
    }

    case 1:
    {
        val = 0x22421219; // green2/blue2 Long

        if (queHandleIndCmdRequest != nullptr)
            xQueueSendToBack(queHandleIndCmdRequest, (void *)&val, pdMS_TO_TICKS(0));
        break;
    }

    case 2:
    {
        val = 0x41411219; // blue1/blue1 Long

        if (queHandleIndCmdRequest != nullptr)
            xQueueSendToBack(queHandleIndCmdRequest, (void *)&val, pdMS_TO_TICKS(0));
        break;
    }

    case 3:
    {
        val = 0x1E001219; // Turn R AUTO

        if (queHandleIndCmdRequest != nullptr)
            xQueueSendToBack(queHandleIndCmdRequest, (void *)&val, pdMS_TO_TICKS(0));
        break;
    }
    }

    // CHANGE YOUR TEST AREA AND INDEX AS NEEDED FOR THE NEXT SEQUENCE YOU WANT

    if (++*index > 3) // We set the limit based on our test sequence
    {
        ESP_LOGW("", "Restarting text index...");
        *index = 0;
    }
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
        break;
    }

    case 1:
    {
        printTaskInfo();
        break;
    }

    case 2:
    {
        while (!xTaskNotify(taskHandleWIFIRun, static_cast<uint32_t>(WIFI_NOTIFY::CMD_DISC_HOST), eSetValueWithoutOverwrite))
            vTaskDelay(pdMS_TO_TICKS(50));

        while (!xTaskNotify(taskHandleWIFIRun, static_cast<uint32_t>(WIFI_NOTIFY::CMD_RUN_DIRECTIVES), eSetValueWithoutOverwrite))
            vTaskDelay(pdMS_TO_TICKS(50));
        break;
    }

    case 3:
    {
        printTaskInfo();
        break;
    }
    }

    // CHANGE YOUR TEST AREA AND INDEX AS NEEDED FOR THE NEXT SEQUENCE YOU WANT

    if (++*index > 3) // We set the limit based on our test sequence
    {
        ESP_LOGW("", "Restarting text index...");
        *index = 0;
    }
}
