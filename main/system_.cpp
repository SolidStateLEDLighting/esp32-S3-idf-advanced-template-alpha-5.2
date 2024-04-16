#include "system_.hpp"

/* Local Semaphores */
SemaphoreHandle_t semSysEntry = NULL;
SemaphoreHandle_t semSysRouteLock = NULL;

/* External Semaphores */
extern SemaphoreHandle_t semNVSEntry;
extern SemaphoreHandle_t semSysBoolLock;
extern SemaphoreHandle_t semSysUint8Lock;
extern SemaphoreHandle_t semSysIndLock;

/* Construction / Destruction */
System::System(esp_reset_reason_t resetReason)
{
    ESP_LOGW(TAG, "Startup...");
    ESP_LOGW(TAG, "Firmware Ver: %d.%d.%d", APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_PATCH);

    resetHandling(resetReason); // Perform any unique action based on how the system (re)started.
    setFlags();                 // Static enabling of logging statements for any area of concern during development.
    setLogLevels();             // Manually sets log levels for tasks down the call stack for development.
    createSemaphores();         // Creates any locking semaphores owned by this object.
    createQueues();             // Create RTOS Commend Request resources.
    restoreVariablesFromNVS();  // Brings back all our persistant data.

    // NOTE: Don't 'take' our own semSysEntry semaphore here, as new objects are calling back to the System for handles throughout initialization.

    sysInitStep = SYS_INIT::Start; // Allow the object to initialize when the task becoming operational
    sysOP = SYS_OP::Init;

    routeLogByValue(LOG_TYPE::INFO, std::string(__func__) + "(): runStackSizek: " + std::to_string(runStackSizeK));
    xTaskCreate(runMarshaller, "sys_run", 1024 * runStackSizeK, this, TASK_PRIORITY_MID, &taskHandleSystemRun);
}

void System::resetHandling(esp_reset_reason_t reason)
{
    // We expect:
    // ESP_RST_POWERON   - This could be a normal Cold boot or a Warm boot via the Reset Switch
    // ESP_RST_DEEPSLEEP - Normal recovery from deep sleep
    //
    // Other restart reasons that we don't catch yet are:
    // ESP_RST_SW        - This may be completely normal.
    // ESP_RST_PANIC     - We may want to log this as an error.
    // ESP_RST_INT_WDT   - We may want to log this as an error.
    // ESP_RST_TASK_WDT  - We may want to log this as an error.
    // ESP_RST_WDT       - We may want to log this as an error.
    // ESP_RST_BROWNOUT  - We may want to log this as an error.

    switch (reason)
    {
    case ESP_RST_POWERON:
    {
        ESP_LOGW(TAG, "Normal Power On...");
        break;
    }

    case ESP_RST_DEEPSLEEP:
    {
        ESP_LOGW(TAG, "Waking from Deep Sleep...");
        break;
    }

    default:
    {
        ESP_LOGW(TAG, "Unhandled Reset number %d", (int)reason); // Ok to read there reset reason again...it remains valid.
        break;
    }
    }
}

void System::setFlags()
{
    // show variable is system wide defined and this exposes for viewing any general processes.
    show = 0; // Set show flags
    show |= _showInit;
    // show |= _showNVS;
    // show |= _showRun;
    // show |= _showEvents;
    // show |= _showJSONProcessing; //
    // show |= _showDebugging;      //
    // show |= _showProcess;        // NOTE: Not all show flags may be used in this object
    // show |= _showPayload;        //       We have them all listed here for consistancy.

    // showSys exposes system sub-processes.
    showSys = 0;
    // showSys |= _showSysTimerSeconds;
    // showSys |= _showSysTimerMinutes;

    diagSys = 0;               // We may be running diagnostics from time to time.
    diagSys |= _diagHeapCheck; // This diag test should run during all development work.
}

void System::setLogLevels()
{
    if ((show + showSys) > 0)                 // Normally, we are interested in the variables inside our object.
        esp_log_level_set(TAG, ESP_LOG_INFO); // If we have any flags set, we need to be sure to turn on the logging so we can see them.
    else
        esp_log_level_set(TAG, ESP_LOG_ERROR); // Likewise, we turn off logging if we are not looking for anything.
}

void System::createSemaphores()
{
    semSysEntry = xSemaphoreCreateBinary(); // External Entry locking
    if (semSysEntry != NULL)
        xSemaphoreGive(semSysEntry);

    semSysRouteLock = xSemaphoreCreateBinary(); // Route locking
    if (semSysRouteLock != NULL)
        xSemaphoreGive(semSysRouteLock);

    semSysBoolLock = xSemaphoreCreateBinary(); // Flag locking
    if (semSysBoolLock != NULL)
        xSemaphoreGive(semSysBoolLock);

    semSysUint8Lock = xSemaphoreCreateBinary(); // Boolean locking
    if (semSysUint8Lock != NULL)
        xSemaphoreGive(semSysUint8Lock);

    semSysIndLock = xSemaphoreCreateBinary(); // Local Indication locking
    if (semSysIndLock != NULL)
        xSemaphoreGive(semSysIndLock);
}

void System::createQueues()
{
    if (systemCmdRequestQue == nullptr)
        systemCmdRequestQue = xQueueCreate(1, sizeof(SYS_CmdRequest *)); // We hold the queue for incoming requests

    if (ptrSYSCmdRequest == nullptr)
        ptrSYSCmdRequest = new SYS_CmdRequest(); // <-- Incoming request

    if (ptrSYSResponse == nullptr)
    {
        ptrSYSResponse = new SYS_Response(); // --> Outgoing responses
        ptrSYSResponse->jsonResponse = nullptr;
    }
}

/* Public Member Functions */
TaskHandle_t System::getRunTaskHandle(void)
{
    return taskHandleSystemRun;
}

QueueHandle_t System::getCmdRequestQueue(void) // Other subsystems need access to the Cmd Request Queue
{
    return systemCmdRequestQue;
}
