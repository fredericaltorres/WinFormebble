@echo off
echo Refresh App: %1
if "%1" =="" goto END

Copy src\*.c samples\%1\src
Copy src\*.h samples\%1\src
Copy src\*.js samples\%1\src

:END