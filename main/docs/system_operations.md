# System Operations

We define a list of system operations in system_enum.hpp.  There currently only 5.  These operations lead to all other sub-operations project wide.

* Run Operation
* Init Operation
* Error Operation
* Idle Operation
* Idle_Silent Operation
![Run Operation Diagram](./drawings/system_operations_block.svg)

### Run Operation
In all objects, a Run is centeral to it's operation.  Most of a task's time is spent here.  The Run operation looks for any RTOS communications, it watches for pending actions, and sometimes state changes.  The object Run task will always return to the centeralize Run operation when waiting is require before the next required action.

### Init Operation
Most objects have an initialization startup requirement.  Initialization typically happens right after the creation of the object and during this time the object is usually locked so the outside world can't interfere with initialization.   At this time, we usually don't look for RTOS communication.

### Error Operation
All Error operations everywhere try to handle any recoverable errors at the level for which they exist.  Usually, errors are forward on until they reach their highest level and they the error message is routed to a message handler.

### Idle Operation
Idle operation is more of a developmental tool where you can route unexepect results.  This operation logs a message before it automatically moves program control into the Idle_Silent operation.

### Idle_Silent Operation
This operation has not activity at all, except for putting this task to sleep for what would be considered long periods of time.