@echo off
REM Set the path to the Cemu executable
set SUYU_PATH=D:\Switch-Suyu\suyu-windows-mingw-20240410-0de49070e4_\suyu.exe
REM Set the path to the .rpx file
set NRO_PATH=%~dp0..\projects.nro

start "" "%SUYU_PATH%" -g "%NRO_PATH%"