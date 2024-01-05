This is a sample boiler plate project for dual core Esp32 microcontroller running FreeRTOS.  This hardware is equivalent to a DevKit-C running an Esp32s3N16R8, but this project could be adapted to other duel core processors.   If run on a single core processor, some of the included features would not be necessary (like varable locking between tasks), but operation should still be normal.

Downloading, compiling, and running the application are all standard common processes and needs no special instruction.  This project only uses the ESP-IDF and is compiled on version 5.2.

>For specific information about components, please exampine doc directories in each respective component area.  

![system_block](./docs/images/project_block.png)

The intent of this sample project is to prepare a suitable development code base for large scale advanced project development.  In pursuit of that goal, this project will evolve to include many other features which are not include here at this time.

Features which are included are:
1. C++ construction
2. Component construction
3. State transition modelling
4. Task Notification (freeRTOS)
5. Queues (which send commands and data)
6. Semaphore locking
7. Basic Wifi connectivity (features)
8. WS2812 Addressable Indication RGB LED (remote control tranceiver RMT driver)

## Abstractions

## Block Diagrams

## Sequence Diagrams

\<end of document>