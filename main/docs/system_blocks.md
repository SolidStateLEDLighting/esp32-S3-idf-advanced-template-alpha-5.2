# System blocks
This is just the system block diagram.  The blocks are in decending alphabetic order so they appear here just as they do inside the project.   Take note that the pattern you see here is universally applied to all object for consistancy.  Not all object contain all the same functions and files, but where possible, we match patterns throughout.  In this way, we can forge a familarity with the design and techniques used to handle tasks and problems.
![System Block Diagram](./drawings/system_block.svg)

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
Just a collection of member functions that are used for diagnostics at a system level.
* Run Time Statistics
* Memory Statistics
* Task Information

### system_gpio.cpp

### system_logging.cpp

### system_nvs.cpp

### system_run.cpp

### system_timer.cpp

### system_utilities.cpp