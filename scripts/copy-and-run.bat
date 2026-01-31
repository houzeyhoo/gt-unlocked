:: For this script to work right, set GT_ROOT env var to the target Growtopia dir
@echo off
setlocal

if not defined GT_ROOT (
    echo Error: GT_ROOT is not defined in the environment
    exit /b 1
)

set "DLL_SOURCE=%~1"
if "%DLL_SOURCE%"=="" (
    echo Warning: No DLL_SOURCE argument provided, skipping copying
)

echo Closing existing game process if one exists...
taskkill /IM "Growtopia.exe" /T >NUL 2>&1
timeout /t 1 /nobreak >NUL

if not "%DLL_SOURCE%"=="" (
    echo Copying %DLL_SOURCE% to %GT_ROOT%\dinput8.dll
    copy /y "%DLL_SOURCE%" "%GT_ROOT%\dinput8.dll"
    if errorlevel 1 (
        echo Error: Copy failed
        exit /b 1
    )
)

echo Starting Growtopia...
start "" /d "%GT_ROOT%" "%GT_ROOT%\Growtopia.exe"

endlocal
