# System Sequences
The creatation of the system object and the generalized process of creating subcomponents (with Tasks and without Tasks) is detailed at the project level [here](../../docs/project_sequences.md)

Sequencing is typically represented as a series of actions between two parties over time.  In the case of software, these parties are either objects or translations units or binary executable components.

In our specific case, we use sequences to represent sequential action between two or more objects.

## Creating Wifi Object
This is a very common pattern where an object with a run task is creating another object with its own run task.  In this case, inside the constructor, the Wifi also creates the non-tasking SNTP object.  
![System Object Creates Wifi Object](../../components/wifi_5.2/src/wifi/docs/drawings/wifi_sequence_calling_constructor.svg)  
___  
## Destroying Wifi Object
___  
## Wifi Connection
___  
## Wifi Disconnection
___   

