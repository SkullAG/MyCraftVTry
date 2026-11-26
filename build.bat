@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

Set includes=/Isrc /I%VULKAN_SDK%/Include  /I%GLFW_SDK%/include
Set links=/link /LIBPATH:%VULKAN_SDK%/Lib /LIBPATH:%GLFW_SDK%/lib-vc2026 vulkan-1.lib glfw3_mt.lib user32.lib gdi32.lib shell32.lib
Set defines=/D DEBUG

echo "Compiling Shaders..."
for /r %%i in ("assets\shaders\*") do (
    if NOT "%%~xi" == ".spv" (
        echo compiling %%i
        "%VULKAN_SDK%\Bin\glslc.exe" %%i -o %%i.spv
    )
) 

echo "Building main..."
cl /std:c++17 /EHsc /Z7 /Fobuild\ /Febuild\main.exe %includes% %defines% *.cpp  %links% 
REM cl /EHsc %includes% %defines% src/main.cpp %links%