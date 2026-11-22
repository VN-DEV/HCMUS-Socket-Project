@echo off
setlocal

echo ========================================================
echo  HCMUS LAN Chat - Automated Build Script
echo ========================================================

if exist "C:\msys64\ucrt64\bin" (
    set "PATH=C:\msys64\ucrt64\bin;%PATH%"
)

where cmake >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake was not found in PATH or C:\msys64\ucrt64\bin.
    echo Please ensure MSYS2 UCRT64 or CMake is installed.
    pause
    exit /b 1
)

echo.
echo [1/2] Generating Ninja build configuration...
cmake -B build -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] CMake configuration failed.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo [2/2] Compiling lan_chat executable...
cmake --build build
if %ERRORLEVEL% neq 0 (
    echo.
    echo [ERROR] Compilation failed.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo ========================================================
echo  Build successful!
echo  Executable created at: build\lan_chat.exe
echo ========================================================
pause
