#include "system_.hpp" // Class structure and variables

extern SemaphoreHandle_t semSysRouteLock;

/* Logging */
void System::routeLogByRef(LOG_TYPE _type, std::string *_msg)
{
    if (xSemaphoreTake(semSysRouteLock, portMAX_DELAY)) // We use this lock to prevent sys_evt and wifi_run tasks from having conflicts
    {
        LOG_TYPE type = _type;   // Copy our parameters upon entry before they are over-written by another calling task.
        std::string *msg = _msg; // This will point back to the caller's variable.

        switch (type)
        {
        case LOG_TYPE::ERROR:
        {
            // ESP_LOGE(TAG, "%s", (*msg).c_str()); // Print out our errors here so we see it in the console.

            // if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
            // {
            //     nvs->writeErrorStringToNVS(msg); // Errors are stored in NVS in case they need to be accessed after a reboot
            //     xSemaphoreGive(semNVSEntry);
            // }
            break;
        }

        case LOG_TYPE::WARN:
        {
            ESP_LOGW(TAG, "%s", (*msg).c_str()); // Print out our warning here so we see it in the console.
            break;
        }

        case LOG_TYPE::INFO:
        {
            ESP_LOGI(TAG, "%s", (*msg).c_str()); // Print out our information here so we see it in the console.
            break;
        }
        }

        xSemaphoreGive(semSysRouteLock);
    }
}

void System::routeLogByValue(LOG_TYPE _type, std::string _msg)
{
    if (xSemaphoreTake(semSysRouteLock, portMAX_DELAY)) // We use this lock to prevent sys_evt and wifi_run tasks from having conflicts
    {
        LOG_TYPE type = _type; // Copy our parameters upon entry before they are over-written by another calling task.
        std::string msg = _msg;

        switch (type)
        {
        case LOG_TYPE::ERROR:
        {
            ESP_LOGE(TAG, "%s", (msg).c_str()); // Print out our errors here so we see it in the console.

            // if (xSemaphoreTake(semNVSEntry, portMAX_DELAY))
            // {
            //     nvs->writeErrorStringToNVS(msg); // Errors are stored in NVS in case they need to be accessed after a reboot
            //     xSemaphoreGive(semNVSEntry);
            // }
            break;
        }

        case LOG_TYPE::WARN:
        {
            ESP_LOGW(TAG, "%s", (msg).c_str()); // Print out our warning here so we see it in the console.
            break;
        }

        case LOG_TYPE::INFO:
        {
            ESP_LOGI(TAG, "%s", (msg).c_str()); // Print out our information here so we see it in the console.
            break;
        }
        }

        xSemaphoreGive(semSysRouteLock);
    }
}
