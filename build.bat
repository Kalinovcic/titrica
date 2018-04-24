@echo off
setlocal
cd %~dp0
path=%~dp0MinGW\bin;%path%
g++ -O2 src/titrica.cpp -o run_tree/titrica run_tree/SDL2.dll run_tree/glew32.dll -lopengl32 -lglu32 -std=c++11 --static -Wl,-subsystem,windows
