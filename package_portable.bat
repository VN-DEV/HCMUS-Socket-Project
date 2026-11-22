@echo off
setlocal enabledelayedexpansion

echo ========================================================
echo  HCMUS LAN Chat - Package Standalone Portable Bundle
echo ========================================================

if exist "C:\msys64\ucrt64\bin" (
    set "PATH=C:\msys64\ucrt64\bin;%PATH%"
)

if not exist "build\lan_chat.exe" (
    echo Building project first...
    call build.bat
)

if not exist "build\lan_chat.exe" (
    echo [ERROR] build\lan_chat.exe not found.
    pause
    exit /b 1
)

echo.
echo [1/3] Preparing dist directory...
if exist "dist" rmdir /s /q dist
mkdir dist
copy /y "build\lan_chat.exe" "dist\" >nul

echo [2/3] Deploying Qt6 libraries with windeployqt...
windeployqt --no-translations dist\lan_chat.exe

echo [3/3] Copying MinGW runtime dependencies and creating ZIP...
python -c "
import os, subprocess, shutil
dist_dir = 'dist'
ucrt_bin = r'C:\msys64\ucrt64\bin'
objdump = os.path.join(ucrt_bin, 'objdump.exe')
copied = set()
while True:
    new_found = False
    files_to_check = [os.path.join(r, f) for r, _, fs in os.walk(dist_dir) for f in fs if f.endswith(('.exe', '.dll'))]
    for f in files_to_check:
        try:
            out = subprocess.check_output([objdump, '-p', f], text=True, stderr=subprocess.DEVNULL)
            for line in out.splitlines():
                if 'DLL Name:' in line:
                    dll = line.split('DLL Name:')[1].strip()
                    dll_lower = dll.lower()
                    if dll_lower.startswith(('api-ms-', 'ext-ms-')): continue
                    if os.path.exists(os.path.join(dist_dir, dll)): continue
                    sys_path = os.path.join(r'C:\Windows\System32', dll)
                    if os.path.exists(sys_path) and not dll_lower.startswith('lib') and not dll_lower.startswith('zlib'): continue
                    ucrt_path = os.path.join(ucrt_bin, dll)
                    if os.path.exists(ucrt_path) and dll not in copied:
                        shutil.copy2(ucrt_path, os.path.join(dist_dir, dll))
                        copied.add(dll)
                        new_found = True
        except: pass
    if not new_found: break
shutil.make_archive('HCMUS_LAN_Chat_Portable', 'zip', 'dist')
print('Portable archive HCMUS_LAN_Chat_Portable.zip created successfully!')
"

echo.
echo ========================================================
echo  Packaging complete: HCMUS_LAN_Chat_Portable.zip
echo ========================================================
pause
