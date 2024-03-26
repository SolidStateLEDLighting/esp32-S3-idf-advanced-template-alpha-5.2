#include "system_.hpp" // Class structure and variables
//
// We bring most logging formation here (inside each object) because in a more advanced project, we route logging
// information back to the cloud.  We could also just as easily log to a file storage location like an SD card.
//
// At is also at this location (in a more advanced project) that I store Error information to Flash.  This makes it possible
// to view or transmit error logging to the cloud after a reboot.
//
/* External Semaphores */
extern SemaphoreHandle_t semSysRouteLock;

/* Logging */
// Logging by reference potentially allows a better algorithm for accessing large data throught a pointer.
void System::routeLogByRef(LOG_TYPE _type, std::string *_msg)
{
    if (xSemaphoreTake(semSysRouteLock, portMAX_DELAY)) // We use this lock to prevent sys_evt and sys_run tasks from having conflicts
    {
        LOG_TYPE type = _type;   // Copy our parameters upon entry before they are over-written by another calling task.
        std::string *msg = _msg; // This will point back to the caller's variable.

        switch (type)
        {
        case LOG_TYPE::ERROR:
        {
            ESP_LOGE(TAG, "%s", (*msg).c_str()); // Print out our errors here so we see it in the console.
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
    if (xSemaphoreTake(semSysRouteLock, portMAX_DELAY)) // We use this lock to prevent sys_evt and sys_run tasks from having conflicts
    {
        LOG_TYPE type = _type; // Copy our parameters upon entry before they are over-written by another calling task.
        std::string msg = _msg;

        switch (type)
        {
        case LOG_TYPE::ERROR:
        {
            ESP_LOGE(TAG, "%s", (msg).c_str()); // Print out our errors here so we see it in the console.
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
