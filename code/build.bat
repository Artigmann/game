@echo off

IF NOT EXIST ..\build mkdir ..\build

set includePath=-I ..\lib\win32\include
set libPath=/LIBPATH:..\lib\win32
set CommonCompilerFlags=-Od -MTd -nologo -fp:fast -fp:except- -Gm- -GR- -EHa- -Zo -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4127 -wd4101 -wd4805 -FC -Z7 %includePath% /EHsc
set CommonLinkerFlags= -incremental:no -opt:ref glew32.lib glfw3dll.lib opengl32.lib

pushd ..\build
cl %CommonCompilerFlags% -D_CRT_SECURE_NO_WARNINGS ..\code\win32_game.cpp /link %libPath% %CommonLinkerFlags%
popd
