@echo off
if NOT exist %cd%\strafenet\ (
    mkdir %cd%\strafenet
    mkdir %cd%\strafenet\include
)
del %cd%\strafenet\** /s /q /f
if exist %cd%\bin\debug\strafenet.lib (
    XCOPY %cd%\bin\debug\strafenet.lib %cd%\strafenet /Y
)
XCOPY %cd%\src\*.h %cd%\strafenet\include /Y