> Taking note of our planned abstraction levels helps us keep the design intent clear between files and functions.

The intent of the System is to be the foundation of the project.  Master control he held here.

---
# Top-Level

## User Classes
Only one class: System

---
# Mid-Level

No mid-level entities.


---
# Low-Level

## ESP-IDF
The system will make calls directly back to the ESP-IDF.  There are no other intermediary actors between the System and the IDF.