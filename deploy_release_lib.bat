@echo off
if NOT exist %cd%\strafenet\ (
    mkdir %cd%\strafenet
    mkdir %cd%\strafenet\include
)
if exist %cd%\strafenet\strafenet-mt.lib (
	del %cd%\strafenet\strafenet-mt.lib /s /q /f
)
if exist %cd%\bin\release\strafenet-mt.lib (
    XCOPY %cd%\bin\release\strafenet-mt.lib %cd%\strafenet /Y
)