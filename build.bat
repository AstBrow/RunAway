@echo off
set PATH=C:\msys64\ucrt64\bin;%PATH%

g++.exe main.cpp -std=c++20 -lraylib -lgdi32 -lwinmm -o game.exe
if %errorlevel% == 0 (
    game.exe
) else (
    pause
)
