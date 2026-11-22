@echo off
if not exist "build\lan_chat.exe" (
    echo Executable not found. Running build.bat first...
    call build.bat
    if not exist "build\lan_chat.exe" exit /b 1
)
start "" "build\lan_chat.exe"
