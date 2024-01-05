> Taking note of our planned abstraction levels helps us keep the design intent clear between files and functions.
---
# Top-Level

### System Class:
The intent of the System is to be the foundation of the project.  Master control is held here.

* Run Loop:

* Timer Loop:

* GPIO Handling

---
# Mid-Level

No mid-level entities.


---
# Low-Level

### ESP-IDF
The system will make calls directly back to the ESP-IDF.  There are no other intermediary actors between the System and the IDF.