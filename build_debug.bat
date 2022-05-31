call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
@echo Building for debug. CWD is:
@echo %cd%
@echo CL building...
cl /Zi /EHsc /nologo /std:c++17 /c ^
/D "_DEBUG" /D "_CONSOLE" ^
/I%cd%\include /Fo: %cd%\obj\debug\ /Fd: %cd%\obj\debug\ /Fa%cd%\obj\debug\ ^
/Od /MTd /MP ^
%cd%\src\*.cpp 

lib /nologo ^
/OUT:%cd%\bin\debug\strafenet.lib %cd%\obj\debug\*.obj /LIBPATH:%cd%\lib ^
enet64.lib ^
ws2_32.lib winmm.lib 
