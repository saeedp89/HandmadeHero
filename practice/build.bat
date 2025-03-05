@echo off
mkdir build
pushd build
cl -Zi ..\practice.cpp user32.lib gdi32.lib

popd
