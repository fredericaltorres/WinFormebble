@echo off
echo.
echo Refresh App: %1
echo ------------------------------------
if "%1" =="" goto END

Copy src\*.c samples\%1\src > null
Copy src\*.h samples\%1\src > null
Copy src\*.js samples\%1\src > null

:END