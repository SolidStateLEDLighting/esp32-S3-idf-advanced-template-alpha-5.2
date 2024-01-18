// These code snippets can be copied and pasted into GPIO and triggered by SW1 for testing.

// NVS Tests
// 1) Boolean read and write
/*
case 0: // Testing of reading and writing a boolean to nvs
{
    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
    {
        ESP_ERROR_CHECK(nvs->openNVSStorage("test"));

        ret = nvs->readBooleanFromNVS("testBool", &testBool);

        if (ret == ESP_OK)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testBool restored as: " + std::to_string(testBool));
        else
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to restore testBool");

        ESP_ERROR_CHECK(nvs->closeNVStorage());
        xSemaphoreGive(semNVSEntry);
    }
    break;
}

case 1:
{
    testBool = !testBool; // toggle the value
    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testBool changed to: " + std::to_string(testBool));
    break;
}

case 2:
{
    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
    {
        ESP_ERROR_CHECK(nvs->openNVSStorage("test"));

        ret = nvs->writeBooleanToNVS("testBool", testBool);

        if (ret == ESP_OK)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testBool saved as: " + std::to_string(testBool));
        else
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to save testBool");

        ESP_ERROR_CHECK(nvs->closeNVStorage());
        xSemaphoreGive(semNVSEntry);
    }
    break;
}
*/

// 2) Read/Writing String to nvs
// std::string testString = "";
// std::string string1 = "The quick brown fox jumps over the lazy dog";
// std::string string2 = "Now is the time for all good men to come to the aid of their country.";
/*
case 0: // Testing of reading and writing an unsigned integer to nvs
{
    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
    {
        ESP_ERROR_CHECK(nvs->openNVSStorage("test"));

        ret = nvs->readStringFromNVS("testString", &testString);

        if (ret == ESP_OK)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testString restored as: " + testString);
        else
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to restore testString");

        ESP_ERROR_CHECK(nvs->closeNVStorage());
        xSemaphoreGive(semNVSEntry);
    }
    break;
}

case 1:
{
    if (testString.compare(string1) == 0)
        testString = string2;
    else
        testString = string1;
    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testString changed to: " + testString);
    break;
}

case 2:
{
    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
    {
        ESP_ERROR_CHECK(nvs->openNVSStorage("test"));

        ret = nvs->writeStringToNVS("testString", &testString);

        if (ret == ESP_OK)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testString saved as: " + testString);
        else
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to write testString");

        ESP_ERROR_CHECK(nvs->closeNVStorage());
        xSemaphoreGive(semNVSEntry);
    }
    break;
}
*/

// 3) uint8_t read and write
/*
case 0: // Testing of reading and writing an unsigned integer to nvs
{
    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
    {
        ESP_ERROR_CHECK(nvs->openNVSStorage("test"));

        ret = nvs->readU8IntegerFromNVS("testUInteger8", &testUInteger8);

        if (ret == ESP_OK)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testUInteger8 restored as: " + std::to_string(testUInteger8));
        else
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to restore testUInteger8");

        ESP_ERROR_CHECK(nvs->closeNVStorage());
        xSemaphoreGive(semNVSEntry);
    }
    break;
}

case 1:
{
    testUInteger8 += 5; // increase the value
    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testUInteger8 changed to: " + std::to_string(testUInteger8));
    break;
}

case 2:
{
    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
    {
        ESP_ERROR_CHECK(nvs->openNVSStorage("test"));

        ret = nvs->writeU8IntegerToNVS("testUInteger8", testUInteger8);

        if (ret == ESP_OK)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testUInteger8 saved as: " + std::to_string(testUInteger8));
        else
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to save testUInteger8");

        ESP_ERROR_CHECK(nvs->closeNVStorage());
        xSemaphoreGive(semNVSEntry);
    }
    break;
}
*/
// 4) int32_t read and write
/*
case 0: // Testing of reading and writing an unsigned integer to nvs
{
    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
    {
        ESP_ERROR_CHECK(nvs->openNVSStorage("test"));

        ret = nvs->readI32IntegerFromNVS("testInteger32", &testInteger32);

        if (ret == ESP_OK)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testInteger32 restored as: " + std::to_string(testInteger32));
        else
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to restore testInteger32");

        ESP_ERROR_CHECK(nvs->closeNVStorage());
        xSemaphoreGive(semNVSEntry);
    }
    break;
}

case 1:
{
    testInteger32 -= 5; // Decrease the value to verify negative numbers for this unsigned variable.
    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testInteger32 changed to: " + std::to_string(testInteger32));
    break;
}

case 2:
{
    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
    {
        ESP_ERROR_CHECK(nvs->openNVSStorage("test"));

        ret = nvs->writeI32IntegerToNVS("testInteger32", testInteger32);

        if (ret == ESP_OK)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testInteger32 saved as: " + std::to_string(testInteger32));
        else
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to save testInteger32");

        ESP_ERROR_CHECK(nvs->closeNVStorage());
        xSemaphoreGive(semNVSEntry);
    }
    break;
}
*/
// 5) uint32_t read and write
/*
case 0: // Testing of reading and writing an unsigned integer to nvs
{
    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
    {
        ESP_ERROR_CHECK(nvs->openNVSStorage("test"));

        ret = nvs->readU32IntegerFromNVS("testUInteger32", &testUInteger32);

        if (ret == ESP_OK)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testUInteger32 restored as: " + std::to_string(testUInteger32));
        else
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to restore testUInteger32");

        ESP_ERROR_CHECK(nvs->closeNVStorage());
        xSemaphoreGive(semNVSEntry);
    }
    break;
}

case 1:
{
    testUInteger32 += 5; // Increase the value
    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testUInteger32 changed to: " + std::to_string(testUInteger32));
    break;
}

case 2:
{
    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
    {
        ESP_ERROR_CHECK(nvs->openNVSStorage("test"));

        ret = nvs->writeU32IntegerToNVS("testUInteger32", testUInteger32);

        if (ret == ESP_OK)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): testUInteger32 saved as: " + std::to_string(testUInteger32));
        else
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to save testUInteger32");

        ESP_ERROR_CHECK(nvs->closeNVStorage());
        xSemaphoreGive(semNVSEntry);
    }
    break;
}
*/

//
// Erasing NVS
//
/*
case 0: // Erases all the partition's variables - defaults to "nvs"
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
*/

/*
case 0: // Erase a single nvs namespace
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

//
// Starting and Stopping a Wifi Connection
//
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