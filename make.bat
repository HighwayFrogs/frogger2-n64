@echo OFF

:: Setup temp directory with copied code.
if not exist build md build
if not exist build\temp md build\temp
del /s build\temp /Q
copy .\Common build\temp
copy .\N64 build\temp
xcopy /e /v /y .\GameData build\temp
copy build\temp\levbanks\levext.cc build\temp\levext.c

:: Setup the path to include the SDK binaries.
SET OLD_PATH=%PATH%
SET PATH=%PATH%;%~dp0SDK\bin
set SN_PATH=%~dp0SDK\bin

:: Make the game.
cd build\temp
make
SET MAKE_ERROR=%ERRORLEVEL%
SET PATH=%OLD_PATH%


:: Verify Frogger 2 executable was made.
if NOT "%MAKE_ERROR%"=="0" goto error
if NOT EXIST block.bin goto error

move block.bin ..\Frogger2.n64
if exist block.sym move block.sym ..\Frogger2.sym
if exist block.map move block.map ..\Frogger2.map
cd ..\..\

goto okay

:error
echo *** There Were Errors ***
cd ..\..\
PAUSE
goto :EOF

:okay
echo Success
PAUSE