# About

This library consists of a small number of .c and .h files, which enable the USB functionality of PIC microcontroller.  
It was created for PIC32MZ (high-speed USB) as Microchip only offer their "harmony"-framework, which imposes a set structure, state-machine and extremely overcomplicates things and is definitely not a joy to use.  
ZorkPicUSB provides USB functionality via API, Callbacks and Interrupts (future).  
A vendor defined interface together with the ZorkLibUSB library allow for simple user-defined data transfer.  

# Known Issues

* **This library is incomplete and work in progress.**
* Library for PIC32MZ2048EFH064, similiar PICs (32MZ) should be easy to port.
* No interrupt handling is supported, a USBService() function has to be called frequently.
* No USB classes are available from the library, only a vendor defined interface with 2 endpoints is configured.
* No WinUSB support for Windows, manual driver installation is required.
* Things are hardcoded in the library and do not depend on the USB configuration.
* The library is not robust to errors and does not check for them.

# Whats even working right now?

* The device enumerates as high-speed device and is successfully recognized from the OS.
* A user-defined interface with 2 endpoints of 512 bytes length are configured.
* These endpoints can be used for data transfer. While they may not be robust to errors, they generally work.
