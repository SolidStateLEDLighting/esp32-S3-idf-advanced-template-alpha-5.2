#include "system_.hpp" // Class structure and variables

#include "esp_check.h"

/* External Variables */
extern SemaphoreHandle_t semNVSEntry;

bool System::restoreVariablesFromNVS()
{
    if (show & _showNVS)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "()");

    uint8_t temp = 0;

    if (nvs == nullptr)
        nvs = NVS::getInstance(); // First, get the nvs object handle if didn't already.

    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY) == pdTRUE)
    {
        if (show & _showNVS)
            ESP_LOGW(TAG, "openNVStorage 'system'");

        if unlikely (!nvs->openNVSStorage("system", true))
        {
            ESP_LOGE(TAG, "Error, Unable to OpenNVStorage inside restoreVariablesFromNVS");
            xSemaphoreGive(semNVSEntry);
            return false;
        }
    }

    bool successFlag = true;

    if (show & _showNVS)
        ESP_LOGW(TAG, "NVS Restore Namespace = system");

    if (successFlag) // Restore runStackSizeK
    {
        temp = runStackSizeK; // This will save the default size if value doesn't exist yet in nvs.

        if (nvs->getU8IntegerFromNVS("runStackSizeK", &temp))
        {
            if (temp > runStackSizeK) // Ok to use any value greater than the default size.
            {
                runStackSizeK = temp;
                saveToNVSDelayCount = 8; // Save it
            }

            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): runStackSizeK is " + std::to_string(runStackSizeK));
        }
        else
        {
            successFlag = false;
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Error, Unable to restore runStackSizeK");
        }
    }

    if (successFlag) // Restore bootCount
    {
        if (nvs->getU32IntegerFromNVS("bootCount", &bootCount))
        {
            if (show & _showNVS)
                ESP_LOGW(TAG, "bootCount is %ld", bootCount);
        }
        else
            successFlag = false;
    }

    if (show & _showNVS)
        ESP_LOGW(TAG, "system end");

    if unlikely (!successFlag)
    {
        ESP_LOGE(TAG, "restoreVariablesFromNVS failed");
        nvs->closeNVStorage(false); // No changes
        xSemaphoreGive(semNVSEntry);
        return false;
    }

    if (show & _showNVS)
        ESP_LOGW(TAG, "restoreVariablesFromNVS Success");

    nvs->closeNVStorage(true); // Commit changes
    xSemaphoreGive(semNVSEntry);
    return true;
}

bool System::saveVariablesToNVS()
{
    if (show & _showNVS)
        ESP_LOGW(TAG, "saveVariablesToNVS");
    //
    // The best idea is to save only changed values.  Right now, we try to save everything.
    //
    if (show & _showNVS)
        ESP_LOGW(TAG, "NVS Save Namespace = system");
    else
    {
        if (xSemaphoreTake(semNVSEntry, portMAX_DELAY) == pdTRUE)
        {
            if unlikely (!nvs->openNVSStorage("system", true)) // Read/Write
            {
                ESP_LOGE(TAG, "Error, Unable to OpenNVStorage inside saveVariablesToNVS");
                xSemaphoreGive(semNVSEntry);
                return false;
            }
        }
    }

    bool successFlag = true;

    if (successFlag)
    {
        if likely (nvs->saveU32IntegerToNVS("bootCount", bootCount))
        {
            if (show & _showNVS)
                ESP_LOGW(TAG, "bootCount = %ld", bootCount);
        }
        else
        {
            ESP_LOGE(TAG, "Error, Unable to saveU32IntegerToNVS bootCount");
            successFlag = false;
        }
    }

    if (show & _showNVS)
        ESP_LOGW(TAG, "system end");

    if (successFlag)
    {
        nvs->closeNVStorage(true); // Commit changes
        xSemaphoreGive(semNVSEntry);
    }
    else
    {
        nvs->closeNVStorage(false); // Discard changes
        xSemaphoreGive(semNVSEntry);
        return false;
    }

    return true;
}
