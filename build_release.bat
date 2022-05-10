call "%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
@echo Building for debug. CWD is:
@echo %cd%
@echo CL building...
cl /Zi /EHsc /nologo /std:c++17 ^
/D "_CONSOLE" /D "GLFW_INCLUDE_NONE" ^
/I%cd%\include /Fo: %cd%\obj\release\ /Fd: %cd%\obj\release\ /Fa%cd%\obj\release\ ^
/O2 /MDd ^
%cd%\src\*.cpp %cd%\src\*.c ^
/link ^
/OUT:%cd%\bin\release\fps.exe /LIBPATH:%cd%\lib ^
glfw3.lib zlibstatic.lib assimp-vc143-mt.lib ^
gdi32.lib advapi32.lib shell32.lib
