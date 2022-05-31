call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
@echo Building for release. CWD is:
@echo %cd%
@echo CL building...
cl /Zi /EHsc /nologo /std:c++17 /c ^
/D "_CONSOLE" ^
/I%cd%\include /Fo: %cd%\obj\release\ /Fd: %cd%\obj\release\ /Fa%cd%\obj\release\ ^
/O2 /MT /MP ^
%cd%\src\*.cpp 

lib /nologo ^
/OUT:%cd%\bin\release\net.lib %cd%\obj\debug\*.obj /LIBPATH:%cd%\lib ^
enet64.lib ^
ws2_32.lib winmm.lib
