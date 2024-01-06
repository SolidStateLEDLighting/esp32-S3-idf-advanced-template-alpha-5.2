#include "system_.hpp"
#include "esp_heap_task_info.h"
#include "esp_heap_caps.h"

void System::printRunTimeStats()
{
    //
    //  DIAGNOSTICS ONLY -- Use this function to see all the tasks running and their load on the system.
    //  This function is automatically defined and comes into scope in tasks.c when
    //
    //  CONFIG_FREERTOS_GENERATE_RUN_TIME_STATS == 1,
    //  CONFIG_FREERTOS_USE_STATS_FORMATTING_FUNCTIONS > 0,
    //  CONFIG_FREERTOS_SUPPORT_STATIC_ALLOCATION == 1
    //
    //  To use this function you must set the following in SKD Configuration editor.
    //  [*] Enable FreeRTOS trace facility
    //  [*] Enable FreeRTOS to collect run time stats
    //  [*] Enable FreeRTOS stats formatting functions
    //
#if ((configGENERATE_RUN_TIME_STATS == 1) && (configUSE_STATS_FORMATTING_FUNCTIONS > 0) && (configSUPPORT_DYNAMIC_ALLOCATION == 1))
    char pcBuffer[800]; // About 40 bytes per task is needed.  Data will be missing if the buffer is short.

    for (int i = 0; i < 3; i++) // We pull data 3 times to show the change in percentage of CPU use.
    {
        vTaskGetRunTimeStats(pcBuffer);
        ESP_LOGW("Task Runtime Stats", "\nTask name       Counter         Percent\n%s\n\n", pcBuffer);
        vTaskDelay(pdMS_TO_TICKS(15000));
    }
#endif
    //
    // Turn off all the config settings when you are done with this because this function stops the schedular for prolonged periods of time.
    // THIS IS A TOOL THAT CAN ONLY BE USED OCCASIONALLY TO GET GENERAL INFORMATION ABOUT THE TASKS.
    //
}

void System::printMemoryStats()
{
    ESP_LOGW(TAG, "Heap MALLOC_CAP_INTERNAL");
    heap_caps_print_heap_info(MALLOC_CAP_INTERNAL);

    ESP_LOGW(TAG, "Heap MALLOC_CAP_SPIRAM");
    heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);

    ESP_LOGW(TAG, "Free heap memory:                %ld bytes", esp_get_free_heap_size());

    ESP_LOGW(TAG, "Minumum free heap:               %d bytes", heap_caps_get_minimum_free_size(MALLOC_CAP_DEFAULT));

    ESP_LOGW(TAG, "Largest default free block:      %d bytes", heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT));
    ESP_LOGW(TAG, "Largest 8/16 bit aligned block:  %d bytes", heap_caps_get_largest_free_block(MALLOC_CAP_8BIT));
    ESP_LOGW(TAG, "Largest 32 bit aligned block:    %d bytes", heap_caps_get_largest_free_block(MALLOC_CAP_32BIT));

    ESP_LOGW(TAG, "Executable memory:               %d bytes", heap_caps_get_largest_free_block(MALLOC_CAP_EXEC));

    ESP_LOGW(TAG, "SPI RAM Largest free block:      %d bytes", heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM));
}

/* Debugging and Testing Routines.  All these will be removed before Distribution */
void System::printTaskInfo()
{
    uint32_t totalNumTasks = uxTaskGetNumberOfTasks();

    printf("...................................................\n");
    printf("  Total Number of Tasks %ld\n", totalNumTasks);
    printf("  name         priority    high water mark\n");

    // char *name = nullptr;
    std::string name = "";
    // std::string name;
    TaskHandle_t hd = nullptr;
    uint32_t priority = 0;
    uint32_t highWaterMark = 0;

    name = "ipc0"; // Inter processor call
    hd = xTaskGetHandle(name.c_str());
    if (hd != NULL)
    {
        priority = uxTaskPriorityGet(hd);
        highWaterMark = uxTaskGetStackHighWaterMark(hd);
        printf("  %-10s   %02ld           %ld\n", name.c_str(), priority, highWaterMark);
    }

    name = "ipc1";
    hd = xTaskGetHandle(name.c_str());
    if (hd != NULL)
    {
        priority = uxTaskPriorityGet(hd);
        highWaterMark = uxTaskGetStackHighWaterMark(hd);
        printf("  %-10s   %02ld           %ld\n", name.c_str(), priority, highWaterMark);
    }

    name = "Tmr Svc";
    hd = xTaskGetHandle(name.c_str());
    if (hd != NULL)
    {
        priority = uxTaskPriorityGet(hd);
        highWaterMark = uxTaskGetStackHighWaterMark(hd);
        printf("  %-10s   %02ld           %ld\n", name.c_str(), priority, highWaterMark);
    }

    name = "esp_timer";
    hd = xTaskGetHandle(name.c_str());
    if (hd != NULL)
    {
        priority = uxTaskPriorityGet(hd);
        highWaterMark = uxTaskGetStackHighWaterMark(hd);
        printf("  %-10s   %02ld           %ld\n", name.c_str(), priority, highWaterMark);
    }

    name = "sys_evt";
    hd = xTaskGetHandle(name.c_str());
    if (hd != NULL)
    {
        priority = uxTaskPriorityGet(hd);
        highWaterMark = uxTaskGetStackHighWaterMark(hd);
        printf("  %-10s   %02ld           %ld\n", name.c_str(), priority, highWaterMark);
    }

    name = "tiT"; // tc/pip task
    hd = xTaskGetHandle(name.c_str());
    if (hd != NULL)
    {
        priority = uxTaskPriorityGet(hd);
        highWaterMark = uxTaskGetStackHighWaterMark(hd);
        printf("  %-10s   %02ld           %ld\n", name.c_str(), priority, highWaterMark);
    }

    // NOTE:  If you call these functions on the Timer task too early, that task will not be created yet.  Instead,
    //        you will see information given for the main task.
    name = pcTaskGetName(taskHandleRunSystemTimer);
    priority = uxTaskPriorityGet(taskHandleRunSystemTimer);
    highWaterMark = uxTaskGetStackHighWaterMark(taskHandleRunSystemTimer);
    printf("  %s   %02ld           %ld\n", name.c_str(), priority, highWaterMark);

    name = pcTaskGetName(taskHandleSystemRun);
    priority = uxTaskPriorityGet(taskHandleSystemRun);
    highWaterMark = uxTaskGetStackHighWaterMark(taskHandleSystemRun);
    printf("  %s   %02ld           %ld\n", name.c_str(), priority, highWaterMark);

    name = pcTaskGetName(runTaskHandleSystemGPIO);
    priority = uxTaskPriorityGet(runTaskHandleSystemGPIO);
    highWaterMark = uxTaskGetStackHighWaterMark(runTaskHandleSystemGPIO);
    printf("  %s   %02ld           %ld\n", name.c_str(), priority, highWaterMark);

    // if (ind != nullptr)
    //     ind->printTaskInfo();

    // if (wifi != nullptr)
    //     wifi->printTaskInfo();

    printf("  IDLE0 and IDLE1 tasks are always priority 0.\n");
    printf("...................................................\n");
}