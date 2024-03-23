#include "system_.hpp"
#include "esp_ota_ops.h"

extern SemaphoreHandle_t semSYSEntry;

extern "C" void app_main(void)
{
    //
    // OTA handling will occur here. (We don't have IOT services added to this project yet)
    //
    const esp_partition_t *bPartition = esp_ota_get_boot_partition();

    if (std::string((char *)bPartition->label) == "factory")
        ESP_LOGW("_main", "We are booting from the factory partition...");
    // else
        // otaPartitionWork();

    // Upon startup, there is always a reset reason.  With a cold boot or the reset button, the startup reason is ESP_RST_POWERON.
    // When we are waking up fron Deep Sleep, the startup reason is ESP_RST_DEEPSLEEP.  We pass in that value so
    // the System to wake up in the correct way according to the startup reason.
    __attribute__((unused)) auto sys = System::getInstance(esp_reset_reason()); // Creating the system singleton object...
}
