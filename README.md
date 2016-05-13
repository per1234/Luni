##LuniLib

###An Arduino library to enable the creation and use of device driver modules with local and remote access.

####Summary

Implements a framework for device drivers for use on the Arduino.

The traditional role of device drivers as an abstraction layer is a very useful one and can be applied in the Arduino environment to bundle device access into a logical, standard, easy to access API.  Each device driver includes methods for open, read, write, and close of **L**ogical **Un**it **I**nstances that represent target device instances. The target device can be physical, virtual, or a combination of the two.

