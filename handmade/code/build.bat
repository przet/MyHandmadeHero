@echo off

mkdir ..\build
pushd ..\build
cl -Zi w:\handmade\code\win32_handmade.cpp user32.lib Gdi32.lib
rem Zi enables debug info. user32.lib eg for MessageBox
popd

