# Project Sequences
At a project level, this sequence explain the general startup process between the System and all other object which have their own task.  Here, the app_main() starts the System and it's 3 tasks, and in turn the System instantiates  

![system_starup_sequence](./drawings/project_startup_pattern_sequence.svg)

**Warning:** The final calls to the component object are dangerous with respect to cross Task (thread) safety. The calls are in most cases just retreiving important RTOS varibles that are needed to setup cross Task sharing of data.   For example, we can't send a Task Notification or send to a Queue without handles to those items.

To mitigate risks, the component initializes those data varibles early before it "gives" its entrySemaphore.   And the calling System object "takes" the component's entrySemaphore before reading those variable handles.   All this typically happens during startup when all system activity is very limited and very predicatble and the result so far have been very trouble free.

This is enough detail for a Project Level description.