#include "system_.hpp"

/* Local Semaphores */
SemaphoreHandle_t semSysEntry = NULL;
SemaphoreHandle_t semSysEventLock = NULL;
SemaphoreHandle_t semSysRouteLock = NULL;

/* External Semaphores */
extern SemaphoreHandle_t semNVSEntry;
extern SemaphoreHandle_t semSysBoolLock;
extern SemaphoreHandle_t semSysUint8Lock;

System::System(void)
{
    setFlags();                // Static enabling of logging statements for any area of concern during development.
    setLogLevels();            // Manually sets log levels for tasks down the call stack for development.
    createSemaphores();        // Creates any locking semaphores owned by this object.
    restoreVariablesFromNVS(); // Brings back all our persistant data.

    // NOTE: Don't 'take' our own semSysEntry semaphore here, as new objects are calling back to the System for handles throughout initialization.

    initSysStep = SYS_INIT::Start; // Allow the object to initialize when the task becoming operational
    sysOP = SYS_OP::Init;
    xTaskCreate(runMarshaller, "sys_run", 1024 * runStackSizeK, this, TASK_PRIORITY_OFFSET_MID, &taskHandleSystemRun);
}

void System::setFlags()
{
    show = 0; // Set show flags
    show |= _showInit;
    // show |= _showNVS;
    // show |= _showRun;
    // show |= _showEvents;
    // show |= _showJSONProcessing; //
    // show |= _showDebugging;      //
    // show |= _showProcess;        // NOTE: Not all show flags may be used in this object
    // show |= _showPayload;        //       We have them all listed here for consistancy.

    showSys = 0;
    // showSys |= _showSysTimerSeconds;
    // showSys |= _showSysTimerMinutes;

    diagSys = 0;               // We may be running diagnostics from time to time.
    diagSys |= _diagHeapCheck; // This diag test should run during all development work.

    iotDirty = 0; // The application always starts out assuming all iot values are not dirty
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

    semSysEventLock = xSemaphoreCreateBinary(); // Event locking
    if (semSysEventLock != NULL)
        xSemaphoreGive(semSysEventLock);

    semSysRouteLock = xSemaphoreCreateBinary(); // Route locking
    if (semSysRouteLock != NULL)
        xSemaphoreGive(semSysRouteLock);

    semSysBoolLock = xSemaphoreCreateBinary(); // Flag locking
    if (semSysBoolLock != NULL)
        xSemaphoreGive(semSysBoolLock);

    semSysUint8Lock = xSemaphoreCreateBinary(); // Boolean locking
    if (semSysUint8Lock != NULL)
        xSemaphoreGive(semSysUint8Lock);
}

TaskHandle_t System::getRunTaskHandle(void)
{
    return taskHandleSystemRun;
}

QueueHandle_t System::getCmdRequestQueue(void) // Other subsystems need access to the Cmd Request Queue
{
    return systemCmdRequestQue;
}
