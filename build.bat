@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

Set includes=/Isrc /I%VULKAN_SDK%/Include
Set links=/link /LIBPATH:%VULKAN_SDK%/Lib vulkan-1.lib
Set defines=/D DEBUG

echo "Building main..."

cl /EHsc %includes% %defines% src/main.cpp %links%