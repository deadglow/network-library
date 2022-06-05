@echo off
if NOT exist %cd%\strafenet\ (
    mkdir %cd%\strafenet
    mkdir %cd%\strafenet\include
)
if exist %cd%\strafenet\strafenet-mtd.lib (
	del %cd%\strafenet\strafenet-mtd.lib /s /q /f
)
if exist %cd%\bin\debug\strafenet-mtd.lib (
    XCOPY %cd%\bin\debug\strafenet-mtd.lib %cd%\strafenet /Y
)