@echo off

:: if not running from the code dir, change it to there! 
set handmade_code_dir=w:\handmade\code
if not %CD%==%handmade_code_dir% pushd %handmade_code_dir% 

:: build in the build dir
mkdir ..\build
pushd ..\build
cl -Zi w:\handmade\code\win32_handmade.cpp user32.lib Gdi32.lib
rem Zi enables debug info. user32.lib eg for MessageBox

:: get out of build dir
popd

:: get back to where we started
popd

