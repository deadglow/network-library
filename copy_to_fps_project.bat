@echo off
if exist %cd%\strafenet\ (
    if NOT exist %cd%\..\cpp-fps-project\include\strafenet\ (
        mkdir %cd%\..\cpp-fps-project\include\strafenet
    )
    del %cd%\..\cpp-fps-project\include\strafenet\** /s /q /f
    xcopy %cd%\strafenet\include\*.h %cd%\..\cpp-fps-project\include\strafenet /y

	if exist %cd%\strafenet\strafenet-mtd.lib (
	    xcopy %cd%\strafenet\strafenet-mtd.lib %cd%\..\cpp-fps-project\lib /y
	) else (
		@echo Debug lib not found.
	)

	if exist %cd%\strafenet\strafenet-mt.lib (
	    xcopy %cd%\strafenet\strafenet-mt.lib %cd%\..\cpp-fps-project\lib /y
	) else (
		@echo Release lib not found.
	)
)