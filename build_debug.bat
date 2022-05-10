call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
@echo Building for debug. CWD is:
@echo %cd%
@echo CL building...
cl /Zi /EHsc /nologo /std:c++17 ^
/D "_DEBUG" /D "_CONSOLE" ^
/I%cd%\include /Fo: %cd%\obj\debug\ /Fd: %cd%\obj\debug\ /Fa%cd%\obj\debug\ ^
/Od /MTd ^
%cd%\src\*.cpp ^
/link ^
/OUT:%cd%\bin\debug\net.exe /DEBUG /LIBPATH:%cd%\lib ^
enet64.lib ^
ws2_32.lib winmm.lib
