# Project Sequences
At a project level, this sequence explains the general startup process between the System and all other objects which run their own tasks.  

The app_main() starts the System.  Each in turn, the System instantiates the remaining supporting components.

**The key thing to observe here is how the System starts another object.**

app_main -> System
The entry point calls sys->getInstance and this task (main task) run through the entire System contructor.

![system_starup_sequence](./drawings/project_startup_self_tasking_sequence_diagram.svg)

**Throughout the project, this pattern is applied to all independant objects.**  An independant object is one with its own running task.  A task-less object is depicted in the next section of this document.

* Step 1: app_main calls getInstance() of the System.  That constructer calls the member functions:
  * setFlags() - Static enabling of logging statements for any area of concern during development.
  * setLogLevels() - Manually sets log levels for tasks down the call stack for development.
  * createSemaphores() - Creates any locking semaphores owned by this object.
  * restoreVariablesFromNVS() - Brings back all our persistant data.
  * Starts a run task (if the object is designed to have a task).

* Step 2: System calls on a class constructor.  The system's run task (thread) runs a set of member functions much like those of the system's constructor.  This pattern stays consistant for the sake of good programming practice.

* Step 3: The created object (typically of a subcomponent) 'takes' its own entry semaphore.  This locks that semaphore.  The purpose during initialization is that the System waits for that semaphore to become available so that it knows the initializaiton process can continue back at its run task.

* Step 4: The created object run an initialization loop of its own to fully initialize.

* Step 5: Created object finishes its initialization and releases the locking semaphore.

* Step 6: Created object begins to operating inside its normal Run loop.

* Step 7: The System, now knowing that the create object is fully initialized, calls back to several "task unsafe" member functions to gather key RTOS handles.  These calls, though unsafe are typically not dangerous either as the variables are written once at the created object and read once by the System object.  The chance for unsafe read/write collisions at start up are impossible at present.

**Warning:** The final calls to the component object are dangerous with respect to cross Task (thread) safety. The calls are in most cases just retreiving important RTOS varibles that are needed to setup cross Task sharing of data.   For example, we can't send a Task Notification or send to a Queue without handles to those items.

To mitigate risks, the component initializes those data varibles early before it "gives" its entrySemaphore.   And the calling System object "takes" the component's entrySemaphore before reading those variable handles.   All this typically happens during startup when all system activity is very limited and very predicatble and the result so far have been very trouble free.

This is enough detail for a Project Level description.