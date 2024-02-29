# System Blocks
This is just the system block diagram.  The blocks are in decending alphabetic order so they appear here just as they do inside the project.   Take note that the pattern you see here is universally applied to all object for consistancy.  Not all object contain all the same functions and files, but where possible, we match patterns throughout.  In this way, we can forge a familarity with the design and techniques used to handle tasks and problems.
![System Block Diagram](./drawings/system_block.svg)

Conceptually, we call this System a component, but in located the 'main' directory, so technically, this is the 'main' component according to the CMake building tool.  

**Take note that we are treating translation units as blocks in our System.  With purpose we have spread out our groups of functions and tasks into these translation units.  Most files are fairly small.  This just a strategy to make it very easy to keep thing in order and easy to find during development**

### system_.cpp
Items which are always handled here:
* Construction
* Destruction
* Setting "show" flag (method of run time tracing)
* Setting log levels.
* Creating semaphores.
* Restoring object values from NVS.
* Starting the Run task.

### system_diagnostics.cpp
Just a collection of member functions that are used for printable diagnostics at a system level.
* Run Time Statistics
* Memory Statistics
* Task Information

### system_gpio.cpp
This translation unit does eveything we need at a system level for all GPIO pins.  It also needs to be understood that some perpherials and their drivers may handle all GPIO pins for their own needs.  This area is used for any and all GPIO pins which are not cared for elsewhere.  Some of thing we handle here are:
* Initialization of GPIO pins.
* Establishment of an RTOS Queue to capture GPIO ISR Events.
* Create the Event handler to act on (or appropriately route) captured GPIO input.
* Starts up the GPIO task which services the RTOS Queue.

### system_logging.cpp
A collections of functions for routing debugging information.  Right now, all logging is done to the serial ports, but later will be routed to cloud storage as well.

### system_nvs.cpp
This is the highest level of abstraction for saving and restoring data from non-volatile storage to the System object.

### system_run.cpp
All our run time loops are contained within the system_run translation unit.  For other object with event handling, there will also be a run-time driven event handling function.

**Our run loop contains all the possible [System Operations](./system_block_operations.md)**

### system_timer.cpp
This translation unit contains a high accuracy esp32 timers and a callback timer handler.  This timer is used for repetative periodic operations.

### system_utilities.cpp
Utility functions can gernalized functions that are shared by several translation unit from within the System object.  

Currently, all our semaphore variable locking functions are located here.   This locking can be important as there are 3 tasks inside the System object and if any of these task share any variables they must be guarded when read from and written to.