# Philosophy

Pwnto-driver is a reimplementation of pwntools in C instead of python. The goal is to fix some pwntools issues. Pwnto-drivers will
 - provide ability to exploit drivers.
 - provide ability to create worm from imported pwnto-driver library.
 
As the dev have some experience with the old pwntools, the new tool aims to fix some known issues:
 - windows program and drivers support. More than pwntools.
 - Linux driver support (more than pwn++ that is written in C++ instead of C).

## alternatives:

 - [pwntools](https://github.com/Gallopsled/pwntools) : Works only on Linux. Not windows. No ability to upload in drivers.
 - [pwn++](https://github.com/hugsy/pwn--) : Does not provide native C support. Then no ability to upload in drivers. 