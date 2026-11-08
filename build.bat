@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

Set includes=/Isrc /I%VULKAN_SDK%/Include
Set links=/link /LIBPATH:%VULKAN_SDK%/Lib vulkan-1.lib user32.lib
Set defines=/D DEBUG

echo "Building main..."

cl /EHsc /Z7 /Fe"main" %includes% %defines% src/platform/win32_platform.cpp %links%
REM cl /EHsc %includes% %defines% src/main.cpp %links%