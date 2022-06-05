@echo off
if NOT exist %cd%\strafenet\ (
    mkdir %cd%\strafenet
    mkdir %cd%\strafenet\include
)
del %cd%\strafenet\include\** /s /q /f
XCOPY %cd%\src\*.h %cd%\strafenet\include /Y