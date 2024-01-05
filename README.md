This is a sample boiler plate project for dual core Esp32 microcontroller running FreeRTOS.  This hardware is equivalent to a DevKit-C running an Esp32s3N16R8, but this project could be adapted to other duel core processors.   If run on a single core processor, some of the included features would not be necessary (like varable locking between tasks), but operation should still appear normal.

Downloading, compiling, and running the application are all standard common processes and needs no special instruction.  This project only uses the ESP-IDF and is compiled on version 5.2.

>For specific information about components, please exampine doc directories in each respective component area.  

![system_block](./docs/images/project_block.png)

The intent of this sample project is to prepare a suitable development code base for large scale advanced project development.  In pursuit of that goal, this project will evolve to include many other features which are not include here at this time.

Features which are included, explained, or demonstrated are:
1. C++ construction
2. Component construction
3. Task Coordination
4. State transition modelling
5. Task Notification (freeRTOS)
6. Queues (which send commands and data)
7. Semaphore locking (code entry and variables)
8. Basic Wifi connectivity
9. WS2812 Addressable Indication RGB LED (employs the remote control transceiver (RMT) driver)

## Abstractions
On a project level, the primary abstraction is the operation of the entire system.  At this point, the project doesn't interact much with the surrounding world, so it's abstraction detail is limited.

## Block Diagrams
The primary block diagram of the project is shown above.  A block diagram shows the building blocks and typically a work process that starts on the left and moves to the right over time.

## Sequence Diagrams

## State Transition Diagrams
One of the basic premises of development in a cooperative multi-tasking system is that the processes (tasks) must yield back to the OS's scheduler on a regular basis frequently enough to supply enough processor time to service all tasks.   If any task doesn't yield, the system will starve of CPU time and the watchdog timer will expire causing a core panic followed by (in most cases) a reboot.

Therefore a development approach must include a smart and effient and easily understood way to complete descrete work, and systematically yield to the OS (in this case freeRTOS).  There may be several approaches to solving this problem, but one well known approach is to use state transition modelling.

State transition modelling provides a well understood mechanism that upon input and time, a system moves from one state to another. In other words, with input, work is acheived, and the result is that the system completes a task and reaches a stopping point.   It is at this stopping point, that we can yield back to the operating system.  When that task resumes work, it has a next task to complete and does what is required before yielding again.  This cycle repeats continously to achieve the system's objectives.  Most of our programming follows this state transition model. 

On the down side, state modelling doesn't look normal to the human eye.  State modelling isn't a linear list of things to do (as is seen in Espressif's code examples).  State modelling resembles a series of inter-related repeating loops.  With time and experience, state modelling becomes easier to see and understand, but one way to more quickly some up to speed with it is to view state modelling diagrams.  As a rule, if you easily diagram your intended state transition process, then it is likely to work nicely as expected.   An explanation as to why you are doing something is typically very important so, always include good documentation about intent.


\<end of document>