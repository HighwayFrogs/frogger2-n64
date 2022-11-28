@echo OFF

:: cppn64 (The preprocessor), errors when there are too many environment variables set.
:: Soooo, the code here will clear them all for the current session.
:: On my computer, the number where it stopped working was 42 environment variables. However, it might be a problem with size of all environment variables combined, not amount.
:: I didn't too much testing since this seemed to work.
for /f "tokens=1* delims==" %%a in ('set') do (
set %%a=
)

:: Setup the path to include the SDK binaries.
SET PATH=%~dp0SDK\bin

:: Setup temp directory with copied code.
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