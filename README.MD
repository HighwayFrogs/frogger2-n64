![Frogger 2 Logo](/_repo/frogger2logo.png)

# Frogger 2: Swampy's Revenge (Cancelled Nintendo 64 Port)  
This is a backup of Frogger 2's N64 source code before the N64 port was cancelled.  
The `main` branch contains a compileable setup which aims to build the December 1, 1999 N64 prototype build as closely as possible.  
The "official" final N64 build seems to have been on December 14th, 1999, so this is less than two weeks from that version.  
However, the December 1st build is preferable because we have a ROM which we can extract assets from.  

## Build Info  
Targetting the December 1st build is a little tough, since there isn't a single commit representing the code used to build the prototype.  
It seems that Andy Eder most likely had some files up to date in VSS and others not. This repository aims to recreate that build as close as possible, and has been done by manually analyzing the ROM to determine which code changes are included and which are not.  
Unfortunately, even we were to perfectly recreate the game code, we cannot byte-match this ROM. This is because either the assembler or the linker used to originally compile the game didn't clear allocated memory.  
So, the ROM contains uninitialized memory, making it near impossible to byte-match without using a decompilation toolchain. Using such a toolchain would require immense work for little benefit.  
The short is that this repository is instead intended to be roughly equivalent when it comes to functionality, instead of byte-matching.  

## How to compile and run?  
Run `make.bat`.  
When it finishes, there will be a N64 ROM in `build\`.  
Compilation has been tested under Windows 10.  

## SDK Information  
The SDK binaries and headers used in this repository are from the SN Systems N64 SDK.  
Most of libraries were converted from the N64 SDK release 2.0J (ELF files) to the SN Systems proprietary .LIB format using the SN Systems conversion program: [ELFCONV.EXE](sdk/bin/ELFCONV.EXE).  
The audio libraries were sourced

**Weird Library Names:**  
Some of the libraries have non-descript names. These file names were what the original developers named the files, which I decided to keep.  
Here's a description of those names.  
```
tonylib2.lib is just 'libgultra.lib' but renamed. No idea why it's called this.
lib314.lib is 'libgmus.a', a libmus library. Version 3.14 according to the libmus.h file included in the repository.
```

## Credits
Preserved by [Highway Frogs](https://highwayfrogs.net/).  
Thanks to [CrashOveride](https://github.com/CrashOveride95) for sourcing the N64 SDK libraries / tools and helping get the code compilable.  