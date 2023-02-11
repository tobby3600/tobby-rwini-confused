@echo off
chcp 65001
del ini_confused.exe
g++ ".\ini_confused.cpp" -o ".\ini_confused.exe" -std=c++14 -fexec-charset=utf-8
".\ini_confused.exe"