@echo off
del c\*.c > NUL

copy UTILS\*.* . > NUL

rem Should only copy the changed ase files
copy ASE\*.ase .

mipple worldconv *.ase c > mipout.bat
call mipout.bat

mipple objconv *.ase c2 > mipout.bat
call mipout.bat

make
linkmake > append.lnk

del *.ase > NUL
del *.obj > NUL
move *.c C > NUL
move *.c2 C > NUL
move *.bin BIN > NUL
move *.bi2 BIN > NUL



call dall

del dall.bat > NUL
del mipout.bat > NUL
