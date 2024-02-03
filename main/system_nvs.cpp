#include "system_.hpp" // Class structure and variables

#include "esp_check.h"

//
// NOTE: We are keeping all variable names to 15 characters or less so that the variables names can also
// be used as key values in non-volitile storage.
//

/* External Semaphores */
extern SemaphoreHandle_t semNVSEntry;

void System::restoreVariablesFromNVS()
{
    esp_err_t ret = ESP_OK;
    bool successFlag = true;
    uint8_t temp = 0;

    if (nvs == nullptr)
        nvs = NVS::getInstance(); // First, get the nvs object handle if we didn't do this previously.

    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
        ESP_GOTO_ON_ERROR(nvs->openNVSStorage("system"), sys_restoreVariablesFromNVS_err, TAG, "nvs->openNVSStorage('system') failed");

    if (show & _showNVS)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): system namespace start");

    if (successFlag) // Restore runStackSizeK
    {
        temp = runStackSizeK;
        ret = nvs->readU8IntegerFromNVS("runStackSizeK", &temp); // This will save the default size if that value doesn't exist yet in nvs.

        if (ret == ESP_OK)
        {
            if (temp > runStackSizeK) // Ok to use any value greater than the default size.
            {
                runStackSizeK = temp;
                ret = nvs->writeU8IntegerToNVS("runStackSizeK", runStackSizeK); // Over-write the value with the default minumum value.
            }

            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): runStackSizeK       is " + std::to_string(runStackSizeK));
        }

        if (ret != ESP_OK)
        {
            successFlag = false;
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Error, Unable to restore runStackSizeK");
        }
    }

    if (successFlag) // Restore gpioStackSizeK
    {
        temp = gpioStackSizeK;                                    // This will save the default size if value doesn't exist yet in nvs.
        ret = nvs->readU8IntegerFromNVS("gpioStackSizeK", &temp); // This will save the default size if that value doesn't exist yet in nvs.

        if (ret == ESP_OK)
        {
            if (temp > gpioStackSizeK) // Ok to use any value greater than the default size.
            {
                gpioStackSizeK = temp;
                ret = nvs->writeU8IntegerToNVS("gpioStackSizeK", gpioStackSizeK); // Over-write the value with the default minumum value.
            }

            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): gpioStackSizeK      is " + std::to_string(gpioStackSizeK));
        }

        if (ret != ESP_OK)
        {
            successFlag = false;
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Error, Unable to restore gpioStackSizeK");
        }
    }

    if (successFlag) // Restore timerStackSizeK
    {
        temp = timerStackSizeK;                                    // This will save the default size if value doesn't exist yet in nvs.
        ret = nvs->readU8IntegerFromNVS("timerStackSizeK", &temp); // This will save the default size if that value doesn't exist yet in nvs.

        if (ret == ESP_OK)
        {
            if (temp > timerStackSizeK) // Ok to use any value greater than the default size.
            {
                timerStackSizeK = temp;
                ret = nvs->writeU8IntegerToNVS("timerStackSizeK", timerStackSizeK); // Over-write the value with the default minumum value.
            }

            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): timerStackSizeK      is " + std::to_string(timerStackSizeK));
        }

        if (ret != ESP_OK)
        {
            successFlag = false;
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Error, Unable to restore timerStackSizeK");
        }
    }

    if (successFlag) // Restore bootCount
    {
        if (nvs->readU32IntegerFromNVS("bootCount", &bootCount) == ESP_OK)
        {
            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): bootCount           is " + std::to_string(bootCount));
        }
        else
        {
            successFlag = false;
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Error, Unable to restore bootCount. Error = " + esp_err_to_name(ret));
        }
    }

    if (show & _showNVS)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): system name end");

    if (successFlag)
    {
        if (show & _showNVS)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): Succeeded");
    }
    else
        routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): restoreVariablesFromNVS Failed");

    nvs->closeNVStorage();
    xSemaphoreGive(semNVSEntry);
    return;

sys_restoreVariablesFromNVS_err:
    routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Error " + esp_err_to_name(ret));
    xSemaphoreGive(semNVSEntry);
}

void System::saveVariablesToNVS()
{
    esp_err_t ret = ESP_OK;
    bool successFlag = true;
    //
    // The best idea is to save only changed values.  Right now, we try to save everything.
    // The NVS object we call will avoid over-writing variables which already hold the correct value.
    // Later, we may try to add and track 'dirty' bits to avoid trying to save a value that hasn't changed.
    //
    if (nvs == nullptr)
        nvs = NVS::getInstance(); // First, get the nvs object handle if we didn't do this previously.

    if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
        ESP_GOTO_ON_ERROR(nvs->openNVSStorage("system"), sys_saveVariablesToNVS_err, TAG, "nvs->openNVSStorage('system') failed");

    if (show & _showNVS)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): system namespace start");

    if (successFlag) // Save runStackSizeK
    {
        if (nvs->writeU8IntegerToNVS("runStackSizeK", runStackSizeK) == ESP_OK)
        {
            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): runStackSizeK       = " + std::to_string(runStackSizeK));
        }
        else
        {
            successFlag = false;
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to writeU8IntegerToNVS runStackSizeK");
        }
    }

    if (successFlag) // Save gpioStackSizeK
    {
        if (nvs->writeU8IntegerToNVS("gpioStackSizeK", gpioStackSizeK) == ESP_OK)
        {
            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): gpioStackSizeK      = " + std::to_string(gpioStackSizeK));
        }
        else
        {
            successFlag = false;
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to writeU8IntegerToNVS gpioStackSizeK");
        }
    }

    if (successFlag) // Save timerStackSizeK
    {
        if (nvs->writeU8IntegerToNVS("timerStackSizeK", timerStackSizeK) == ESP_OK)
        {
            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): timerStackSizeK     = " + std::to_string(timerStackSizeK));
        }
        else
        {
            successFlag = false;
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to writeU8IntegerToNVS timerStackSizeK");
        }
    }

    if (successFlag) // Save bootCount
    {
        if (nvs->writeU32IntegerToNVS("bootCount", bootCount) == ESP_OK)
        {
            if (show & _showNVS)
                routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): bootCount           = " + std::to_string(bootCount));
        }
        else
        {
            successFlag = false;
            routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Unable to writeU32IntegerToNVS bootCount");
        }
    }

    if (show & _showNVS)
        routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): system namespace end");

    if (successFlag)
    {
        if (show & _showNVS)
            routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): saveVariablesToNVS Succeeded");
    }
    else if (show & _showNVS)
        routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): saveVariablesToNVS Failed");

    nvs->closeNVStorage();
    xSemaphoreGive(semNVSEntry);
    return;

sys_saveVariablesToNVS_err:
    routeLogByValue(LOG_TYPE::ERROR, std::string(__func__) + "(): Error " + esp_err_to_name(ret));
    xSemaphoreGive(semNVSEntry);
}
