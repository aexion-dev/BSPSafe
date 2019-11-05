========================
BSPSafe v1.0.0
========================

BSPSafe is designed to obfuscate .bsp files and prevent them from being decompiled through BSPSource/VMEX.

The program has been written in C++ and can be compiled in Visual Studio 2015. After compiled, drag a .bsp onto the .exe and a new file will be generated in the same directory as the original.

This is only intended for use with CS:GO but may be adaptable to earlier source-engine games if you recompile without the check for version 21. 

BSPSafe works against BSPSource 1.3.21 and VMEX 0.98g as of 4/23/2016. Future versions of BSPSource will more than likely attempt to counteract the obfuscation (Please don't patch this Neko!!).

Credits to Omni for the initial idea and guidance on best practices when obfuscating maps.