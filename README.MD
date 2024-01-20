![Frogger 2 Logo](/_repo/frogger2logo.png)

# Frogger 2: Swampy's Revenge (Cancelled Nintendo 64 Port)  
This is a backup of Frogger 2's N64 source code before the N64 port was cancelled.  
The `main` branch contains a compileable setup which aims to build the December 1, 1999 N64 prototype build as closely as possible.  
The "official" final N64 build seems to have been on December 14th, 1999, so this is less than two weeks from that version.  
However, the December 1st build is preferable because we have a ROM which we can extract assets from.  

## Build Info  
Targetting the December 1st build is a little tough, since there isn't a single commit representing the code used to build the prototype.  
It seems that Andy Eder most likely had some files up to date in VSS and others not. This repository aims to recreate that build as close as possible, and has been done by manually analyzing the ROM to determine which code changes are included and which are not.  
The original ROM also contains unallocated/garbage memory used as padding bytes between compiler objects.  
It is not clear why this data is there, but likely was due to a buggy version of psylink.  
If the garbage bytes are ignored, this repository builds a perfect match of that prototype ROM.  
Target SHA1 Hash: `3895959f2a8c31481a29d42ab7b8e7eddce95df5`  

## How to compile and run?  
Run `make.bat`.  
When it finishes, there will be a N64 ROM in `build\`.  
Compilation has been tested under Windows 10.  

## SDK Information  
The SDK binaries and headers used in this repository are from the SN Systems N64 SDK.  
Most of libraries were converted from the N64 SDK release 2.0J (ELF files) to the SN Systems proprietary .LIB format using the SN Systems conversion program: [ELFCONV.EXE](sdk/bin/ELFCONV.EXE).  
The audio libraries are libmus, and were sourced from the libmus distributions.  

**Weird Library Names:**  
Some of the libraries have non-descript names. These file names were what the original developers named the files, which I decided to keep.  
Here's a description of those names.  
```
tonylib2.lib is just 'libgultra.lib' but renamed. No idea why it's called this.
lib314.lib is 'libgmus.a', a libmus library. Version 3.14 according to the libmus.h file included in the repository.
```

**SDK Binaries:**  
```
Libraries:
sn-sys.tar\www.snsys.com\snsys\files\n64win95\libsn.zip\LIB\
[1998-03-24] LIBSN.LIB
[1998-03-24] PSYQ.OBJ
[1998-03-24] PSYQDBG.OBJ
[1998-03-24] SNMAIN.S (Modified to byte-match)

Host Executables:
[1998-01-16] asmn64.exe: Human Assembler v2.47, sn-sys.tar\www.snsys.com\snsys\files\n64win95\ASMN64.ZIP
[1998-10-02] asn64.exe: Machine Assembler Version 2.81. sn-sys.tar\www.snsys.com\snsys\files\n64win95\ASN64.ZIP
[1998-06-09] cc1n64.exe: GCC 2.8.1 SN32 BUILD 4.0.0010, sn-sys.tar\www.snsys.com\snsys\files\n64win95\gnun64281.ZIP
[1998-06-09] ccpn64.exe: GCC C Preprocessor 2.8.1, sn-sys.tar\www.snsys.com\snsys\files\n64win95\gnun64281.ZIP
[1998-09-22] ccn64.exe: Equivalent of GCC.EXE v3.06.0017, sn-sys.tar\www.snsys.com\snsys\files\n64win95\CCN64.ZIP
[1996-12-12] elfconv.exe: Used to convert the official N64 SDK libraries in ELF format to SN Systems' format.
[1999-04-20] flatpack.exe: The proprietary ISL/Blitz Games tool used to compress files in the FLA2 format. Obtained Chicken Run EOP Tape 1, Session 2.
[2024-01-18] header64.exe: A tool by Kneesnap to write the game name / game code to an N64 ROM header. Source code included in sdk/src.
[????-??-??] make.exe: Some variant of make. Don't recall where this came from.
[????-??-??] makemask.exe: Creates the IPLv3 boot code (Data between 0x40 and 0x1000). From CrashOveride
[1998-09-14] psylink.exe: SN Linker v2.79, used to combine all of the objs into an N64 ROM.
[????-??-??] setcsum.exe: Generates the checksum required to make the N64 ROM work properly. Probably from CrashOveride.
```

## Credits
Preserved by [Highway Frogs](https://highwayfrogs.net/).  
Thanks to [CrashOveride](https://github.com/CrashOveride95) for sourcing the N64 SDK libraries / tools and helping get the code compilable.  
Thanks to [Mr-WiseGuy](https://github.com/RocketRet) helping diagnose the compiler flag `-g` needing to be `-g1`.  