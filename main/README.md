# System (Main) Component
The system is made up of one object and three tasks.  This object is a singleton and has no destructor.  The system can only be restarted upon a full shutdown and reboot of the hardware.

The system mangages everything at a high level.  It does the following:
* Initalizes all components.
* Has the power to shutdown and restart components (objects).
* May open and close external communications.
* Triggers periodic system-wide tasks.
* Handles error recovery on a system scale (if possible).
* It is in charge of putting the system to sleep. **(not yet impliemented)**

The following categories will help you visualize different aspects of the system:

1) [System Abstractions](./docs/system_abstractions.md)
2) [System Block Diagram](./docs/system_blocks.md)
3) [System Flowcharts](./docs/system_flowcharts.md)
4) [System Operations](./docs/system_operations.md)
5) [System Sequences](./docs/system_sequences.md)
6) [System State Models](./docs/system_state_models.md)
___  