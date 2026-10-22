@echo off
setlocal
cd /d "%~dp0.."
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
if not exist benchmarks\Release mkdir benchmarks\Release
cl /nologo /O2 /DNDEBUG /EHsc /std:c++17 /MD /D NOMINMAX /I sdk_src/packages/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/include benchmarks/SpriteBatchTests.cpp /Fo:benchmarks/Release/SpriteBatchTests.obj /Fe:benchmarks/Release/SpriteBatchTests.exe /link d3d9.lib gdi32.lib user32.lib /libpath:sdk_src/packages/Microsoft.DXSDK.D3DX.9.29.952.8/build/native/release/lib/x64 d3dx9.lib
if errorlevel 1 exit /b 1
set "PATH=%CD%\sdk_src\packages\Microsoft.DXSDK.D3DX.9.29.952.8\build\native\release\bin\x64;%PATH%"
benchmarks\Release\SpriteBatchTests.exe
