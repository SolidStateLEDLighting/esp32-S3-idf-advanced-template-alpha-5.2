## Adjusting Maximum Priorty Value

This project "task tunes" the range of priorities for RTOS.  The default range for task priorities is 0 through 25.  Priority 25 is the highest priority were 0 is the lowest.

The working theory on tuning would be to reduce the full range of priorities down to the very minimum number of priorities in actual use so that compare and branching instructions run in micro-code would result in the bare minumum number of instruction cycles used when the schedular looks for the next task to run.

On the down-side, after you optimize the full number range downward, there is always a chance in the future that you may need to revisit your optimization again and change it.

In this project we have 14 tasks.  With the assertion that configMAX_PRIORITIES is set to 7, their prorites are assigned as follow:

| Task      | Calculations                               | Value |
| :-------- | :----------------------------------------- | :---: |
| ipc0      | configMAX_PRIORITIES - 1                   |   6   |
| ipc1      | configMAX_PRIORITIES - 1                   |   6   |
| wifi      | configMAX_PRIORITIES - 2                   |   5   |
| tit       | manually assigned in menuconfig            |   5   |
| esp_timer | configMAX_PRIORITIES - 3                   |   4   |
| tmr_svc   | manually assigned in menuconfig            |   3   |
| sys_tmr   | manually assigned configMAX_PRIORITIES - 4 |   3   |
| sys_evt   | configMAX_PRIORITIES - 5                   |   2   |
| sys_run   | manually assigned configMAX_PRIORITIES - 5 |   2   |
| sys_gpio  | manually assigned configMAX_PRIORITIES - 5 |   2   |
| wifi_run  | manually assigned configMAX_PRIORITIES - 5 |   2   |
| ind_run   | manually assigned configMAX_PRIORITIES - 6 |   1   |
| IDLE0     | set by freeRTOS                            |   0   |
| IDLE1     | set by freeRTOS                            |   0   |

### Instructions:
If you installed the IDF in the default location, then you can find the header file here:  
C:\Users\<user name>\esp\v5.2\esp-idf\components\freertos\config\include\freertos\FreeRTOSConfig.h

1) Open the FreeRTOSConfig.h file and edit     #define configMAX_PRIORITIES (25)    -- change 25 to the value 7