# Project Blocks
Here again is our project block diagram.  Below, we will provide some descriptions and links to drill-down into the components of interest.
![system_block](./drawings/project_block.svg)
### Main:
The main is just the app entry point and doesn't do much for the application as a whole.  Shortly after starting the system by calling the system's constructor, the main exits.  Main does nothing else at this time.   All the task memory for main is reclaimed once that task ends (which is a benefit for exiting app_main()).

### System:
The System is a singleton object and remains resident and active for the entire lifetime of the application.  The System will only be destroyed and restarted upon a hardware reboot.  From a practical perspective, the System owns all the other objects and controls their lifecycles.

**The System contains 3 task driven core services:**
* **GPIO**
The **system_gpio** service runs a task that initializes and manages GPIO and responds to GPIO interrupts.  Any number of pins and pin-driven interupts can be handled or monitored here.  It is worth mentioning that many peripherals will initialize their own pins and handle their own interrupts without the need for this service.  Some attention and thought should be given to initializing unused pins so they are effectly neutral during operation and possible sleep states. 

* **Run**
The **system_run** service sends and received all our RTOS bases inter-task communication.  This task based service is loosely considered the "super loop" for the entire system.  **system_run** spins up all the objects, handles centeralized Task Notification, Command Queue, and can make system wide decisions on operation based on various input.  **system_run** is also the service that would take the entire system into and out of sleep states (currently not implemented).  Other freeRTOS communication mechnisms like Streams, Message Buffers, Mutexes are not demonstrated here at this time.

* **Timer**
The **system_timer** supplies a timer service that offers a highly accurate way to trigger actions on a periodic basis.  Action can be taken in seconds, minutes, hours even possibly days or even months provided that the timer loop continues to run for the designated period.  The majority of work is done on fairly short time lengths (less than 1 minute).

    Currently the **system_timer** handles:
    - Switch input debouncing
    - LED heartbeat indication (if enabled in code)
    - NVS storage action delay (saves varibles after a prescribed number of seconds)
    - Runs periodic diagnostics

## Components:

### Wifi:
The Wifi component handles all the communication to Network Interface.  It may also optionally make calls to SNTP.  In the futre, the Wifi component will also handle Wifi Provisioning, and Wifi Mesh networking.

### Indication:
The Indication component controls an exteral LED indicator for feedback to the user.  Any object inside the system may supply a Command sent to a Queue in the Indication object to trigger a 1 or 2 number flashing code in variety colors.  The brightness of the ws2812 LED color channels can also be set by a Task Notification command (and remembered in nvs).   As a final option, single colors can be permanently turned on or off (also part of a Task Notification command).  The benefit of this component is that complete control of the RBG LED can be taken through simple commands. 

NOTE: At start-up, this object outputs the firmware version number in RGB color.  Red for MAJOR, Green for MINOR, Blue for PATCH number of the firmware.  For the sake of reading the version easily, all version numbers should be between 1 and 6.

This version of the indication object is programmed to operate a 1 wire output using the RMT (remote control transceiver) driver to control an WS2812 LED.   Other versions of Indication component might function with other types of LED indicators (non-ws2812 chip).  For example, LEDs could be attached to simple GPIOs directly (software would need to be modified to accomodate this).

### NVS
The non-volatile storage component is a middle abstraction object for accessing the ESP's non-volatile flash storage.  It is an extension object which by itself has no running task.  A calling object first takes a semaphore to lock access to nvs, and then calls on its function for nvs activity.   This object is a singleton (like the System) and it never shuts down under normal circumstances.

## Tasks
Behind the scenes the IDF starts a number of tasks.  A complete task list can be generated at run time with the printRunTimeStats() function which is located in system_diagnostics.cpp   printRunTimeStats() is a special function and you'll need to be aware that it should only be used on a limited basis as it puts a burden on the scheduler.

### IDLE Tasks:
IDLE tasks are freeRTOS processes which run when a CPU cores are in the idle state.   Each IDLE task (IDLE0 and IDLE1) is pinned to their respective cores.  Priority for IDLE tasks are always 0 (lowest priority).

### ipc Tasks:
ipc stands for Inter Processor Communication.

### sys_evt Task:
The default system event loop creates this task to handle events on that loop.  When a call-back for an event is invoked, this task runs that call-back.

### tiT Task:
This is the main task for the TCP-IP stack (LwIP).

### Tmr Svc Task:
This is the primary freeRTOS Daemon Task.

### esp_timer:
This task is created in response to any esp timer tasks that you create.  This Task is the one that makes a timer call-back.

### Other Tasks:
Other than the list above, most other tasks are created in code.  These will be servicing objects in system created components (in our case, our Wifi, nvs, and Indication objects).

## ESP-IDF:
This is the primary library that Espressif creates to provide you all the services that you need, but where is Arduino?  Now that you're an expert, or becomming an expert, you won't need an intermedate library like Arduino, MicroPython, MongooseOS or (fill in the blank).  Intermediate libraries can add trouble with software bugs that you just can't solve.  If you can accept the IDF library, then the whole world is your oyster, so it is up to you to master key software concepts and produce the best possible product with Esp32 hardware.  If you can master the IDF, then no one on the planet can make a smaller, faster, better, Esp32 application than you can.