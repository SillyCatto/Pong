@echo off
g++ main.cpp -Wall -I .\raylib\include -L .\raylib\lib -lraylib -lopengl32 -lgdi32 -lwinmm -std=c++17 -static -o pong.exe
echo.
echo.
echo Process returned %errorlevel%
