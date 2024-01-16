// These code snippets can be copied and pasted into GPIO and triggered by SW1 for testing.

// NVS Tests
// 1) Boolean read and write
/*
case 0: // Testing of reading and writing a boolean to nvs
{
    if (nvs->openNVSStorage("test", true) == false) // Read/Write
        routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Error, Unable to nvs->openNVStorage()");

    if (nvs->getBooleanFromNVS("autoConnect", &autoConnect))
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): autoConnect restored = " + std::to_string(autoConnect));
    else
        routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Error, Unable to restore autoConnect");
    nvs->closeNVStorage(true); // Commit changes
    break;
}

case 1:
{
    autoConnect = !autoConnect;
    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): autoConnect changed to " + std::to_string(autoConnect));
    break;
}

case 2:
{
    if (nvs->openNVSStorage("test", true) == false) // Read/Write
        routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Error, Unable to nvs->openNVStorage()");

    if (nvs->saveBooleanToNVS("autoConnect", autoConnect))
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): autoConnect saved   = " + std::to_string(autoConnect));
    else
        routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to save autoConnect");
    nvs->closeNVStorage(true); // Commit changes
    break;
}
*/

// 2) Erasing NVS
/*
case 0: // Erases all the partition's variables - defaults to "nvs"
{
    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
    {
        ESP_LOGW(TAG, "eraseNVSPartition");
        nvs->eraseNVSPartition();
        xSemaphoreGive(semNVSEntry);
    }
    break;
}
*/

/*
case 0: // Only erases a single nvs namespace
{
    std::string strNameSpace = "wifi";
    // std::string strNameSpace = "system";
    // std::string strNameSpace = "sntp";
    // std::string strNameSpace = "indication";

    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
    {
        ESP_LOGW(TAG, "eraseNVSNamespace %s", strNameSpace.c_str());
        nvs->eraseNVSNamespace((char *)strNameSpace.c_str()); // Erases all the variables in this namespace
        xSemaphoreGive(semNVSEntry);
    }
}
*/

// Starting and Stopping a Wifi Connection
/*
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
*/