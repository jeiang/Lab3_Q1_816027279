# ECNG3006 Lab 3 Question 1

## Unit testing
Approach:
- Own Structure
This approach was chosen due to the following reasons:
- Test Harness
  - Unity Framework
    This was not used as the Unity Framework was designed for components, and the example that was used was not a
    component. Modifying the code to become a component was deemed to be impractical for the scale of the lab.
    Additionally, the default configuration required a larger flash size than was available for the ESP-01S.
    There is also no documentation for using it with the ESP8266 RTOS SDK.
  - Other Frameworks (including GoogleTest). 
    These would have required significant adjustment to the makefiles to support the ESP8266, or allow for x86 emulation
    with stubs.
- Manual Configuration
  This was chosen due to its simplicity, as well as the speed of development. It also allowed for learning how input works
  on the ESP8266.

## Integration Testing
A driver was not used, as the scale of testing was small, and it would have been "overkill" to use a driver. However,
a driver would have allowed for better scaling of the code, and to separate the code from the tests better. 

## Verfication Testing
The aspect that was being verified was the configuration of the pins. The test confirmed that the pins operated correctly
and they were able to trigger the interrupt correctly.
