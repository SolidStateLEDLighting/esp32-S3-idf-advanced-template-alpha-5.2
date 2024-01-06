# Project Blocks
Here again is our project block diagram.  We are also including some detail on what would be considered the system object.

![system_block](./images/project_block.png)

### Main:
The main is just the app entry point and doesn't do much for the application.  Shortly after starting the system, the main exits and it's task memory is reclaimed.  Main does nothing else at this time.

### System:
The System is a singleton object and remains resident and active for the entire lifetime of the application.  The System will only be destroyed and restarted upon a hardware reboot.

The System contains 3 core services:
* GPIO
* Run
* Timer



### Indication:
The Indication object controls an exteral LED indicator for feedback to the user.  Any object inside the system may supply a Task Notification to the Indication object to trigger a 1 or 2 number flashing code in variety colors.

At start-up, this object outputs the firmware version number in RGB color.  Red for MAJOR, Green for MINOR, Blue for REVISION number of the firmware.

This version of the indication object is programmed to operate a 1 wire output using the RMT (remote control transceiver) driver to an WS2812 LED.

### Wifi:


### ESP-IDF:

## Basic function

## Tasks
Behind the scenes, the IDF starts a number of tasks which are important to be aware of.   A complete task list can be generated with the printRunTimeStats() function which is located in system_diagnostics.

### IDLE Tasks:
IDLE tasks run when a CPU core is in the idle state.  Priority for IDLE tasks are always 0 (lowest priority).

### ipc Tasks:
ipc stands for Inter Processor Communication.

### sys_evt Task:

### tiT Task:

### Tmr Sve Task:

### esp_timer: