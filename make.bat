@echo OFF

SET PATH=%~dp0SDK\bin

REM Setup temp directory with copied code.
if not exist BUILD md BUILD
if not exist BUILD\TEMP md BUILD\TEMP
if not exist BUILD\TEMP\levbanks md BUILD\TEMP\levbanks
del BUILD\TEMP /Q
copy .\Common BUILD\TEMP
copy .\N64 BUILD\TEMP
copy .\GameData\levbanks BUILD\TEMP\levbanks

:: Make the game.
cd BUILD\TEMP
make

:: Verify Frogger 2 executable was made.
if errorlevel 1 goto error
if NOT EXIST block.bin goto error

move block.bin ..\Frogger2.n64
cd ..\

goto okay

:error
echo *** There Were Errors ***
PAUSE
goto :EOF

:okay
echo Success
PAUSE