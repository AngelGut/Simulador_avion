@echo off
REM Copy DLLs from vcpkg to output directory
REM Usage: copy_dlls.bat OUTPUT_DIR

set OUTDIR=%1
set VCPKG_ROOT=C:\Users\angel\Desktop\vcpkg

REM Try to copy from packages directory (most reliable)
if exist "%VCPKG_ROOT%\packages\glew_x64-windows\bin\glew32.dll" (
    copy /Y "%VCPKG_ROOT%\packages\glew_x64-windows\bin\glew32.dll" "%OUTDIR%" >nul 2>&1
)

if exist "%VCPKG_ROOT%\packages\assimp_x64-windows\bin\assimp-vc145-mt.dll" (
    copy /Y "%VCPKG_ROOT%\packages\assimp_x64-windows\bin\assimp-vc145-mt.dll" "%OUTDIR%" >nul 2>&1
)

REM Fallback to installed directory
if not exist "%OUTDIR%\glew32.dll" (
    if exist "%VCPKG_ROOT%\installed\x64-windows\bin\glew32.dll" (
        copy /Y "%VCPKG_ROOT%\installed\x64-windows\bin\glew32.dll" "%OUTDIR%" >nul 2>&1
    )
)

if not exist "%OUTDIR%\assimp-vc145-mt.dll" (
    if exist "%VCPKG_ROOT%\installed\x64-windows\bin\assimp-vc145-mt.dll" (
        copy /Y "%VCPKG_ROOT%\installed\x64-windows\bin\assimp-vc145-mt.dll" "%OUTDIR%" >nul 2>&1
    )
)

exit /b 0
