# freeRTOS Changes

## Adjusting Maximum Priorty Values

This project "task tunes" the range of priorities for RTOS.  The default range for task priorities is 0 through 25.  Priority 25 is the highest priority whereas 0 is the lowest.

The working theory on tuning would be to reduce the full range of priorities down to the very minimum number of priorities in actual use so that compare and branching instructions run in micro-code would result in the bare minumum number of instruction cycles used when the scheduler looks for the next task to run.

On the down-side, after you optimize the full number range downward, there is always a chance in the future that you may need to revisit your optimization again and expand it.

In this project we have 14 tasks.  With configMAX_PRIORITIES is set to 7, their prorites are assigned as follow:

| Task      | Calculation                                         | Value |
| :-------- | :-------------------------------------------------- | :---: |
| ipc0      | configMAX_PRIORITIES - 1 (automatic)                |   6   |
| ipc1      | configMAX_PRIORITIES - 1 (automatic)                |   6   |
| wifi      | configMAX_PRIORITIES - 2 (automatic)                |   5   |
| tit       | manually assigned in menuconfig                     |   5   |
| esp_timer | configMAX_PRIORITIES - 3 (automatic)                |   4   |
| tmr_svc   | manually assigned in menuconfig                     |   3   |
| sys_tmr   | manually assigned in code TASK_PRIORITY_OFFSET_HIGH |   3   |
| sys_evt   | configMAX_PRIORITIES - 5 (automatic)                |   2   |
| sys_run   | manually assigned in code TASK_PRIORITY_OFFSET_MID  |   2   |
| sys_gpio  | manually assigned in code TASK_PRIORITY_OFFSET_MID  |   2   |
| wifi_run  | manually assigned in code TASK_PRIORITY_OFFSET_MID  |   2   |
| ind_run   | manually assigned in code TASK_PRIORITY_OFFSET_LOW  |   1   |
| IDLE0     | set by freeRTOS                                     |   0   |
| IDLE1     | set by freeRTOS                                     |   0   |

### Instructions:
If you installed the IDF in the default location, then you can find the header file here:  

*C:\Users\<user name>\esp\v5.2\esp-idf\components\freertos\config\include\freertos\FreeRTOSConfig.h*

1) Open the FreeRTOSConfig.h file and edit     #define configMAX_PRIORITIES (25)    -- changing 25 to the value 7
2) Save all, full clean project, rebuild.  


**NOTE:** "tmr_svc" is a name that has been manually set in menuConfig for **configTIMER_SERVICE_TASK_NAME**