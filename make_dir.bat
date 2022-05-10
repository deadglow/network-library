@echo off
if NOT exist %cd%\obj\ (
    mkdir %cd%\obj
    mkdir %cd%\obj\debug
    mkdir %cd%\obj\release
    echo Obj directories created
) else (
    echo Obj directories exist
)

if NOT exist %cd%\bin\ (
    mkdir %cd%\bin
    mkdir %cd%\bin\debug
    mkdir %cd%\bin\release
    echo Bin directories created
) else (
    echo Bin directories exist
)

if NOT exist %cd%\working\ (
    mkdir %cd%\working
    echo Working directory created
) else (
    echo Working directory exists
)