# BSPSafe v1.0.0

BSPSafe is designed to obfuscate .bsp files and prevent them from being decompiled/stolen through BSPSource/VMEX.

The program has been written in C++ and can be compiled in Visual Studio 2015. Drag and drop a .bsp onto the compiled .exe and a new obfuscated map will be generated in the same directory as the original.

BSPSafe works against [BSPSource 1.3.21](https://developer.valvesoftware.com/wiki/BSPSource) and [VMEX 0.98g](http://www.bagthorpe.org/bob/cofrdrbob/vmex.html) as of 4/23/2016. Future versions of BSPSource will more than likely attempt to counteract the obfuscation (*Please don't patch this Neko!!*).

This is only intended for use with CS:GO but may be adaptable to earlier source-engine games by recompiling without the check for version 21. 

Credits to @omnicoder for help with the initial build and guidance on best practices when obfuscating maps.
