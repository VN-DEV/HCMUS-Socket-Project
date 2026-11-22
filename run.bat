@echo off
if exist "lan_chat.exe" (
    start "" "lan_chat.exe"
    exit /b 0
)
if exist "runtime\lan_chat.exe" (
    start "" "runtime\lan_chat.exe"
    exit /b 0
)
if not exist "build\lan_chat.exe" (
    echo Executable not found. Running build.bat first...
    call build.bat
    if not exist "build\lan_chat.exe" exit /b 1
)
start "" "build\lan_chat.exe"
