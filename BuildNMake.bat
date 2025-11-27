@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
echo.

echo Building NMake (Simulator)...
echo.

cd build.nmake
nmake
if %ERRORLEVEL% neq 0 (
    cd ..
    echo.
    echo Build failed!
    pause
    exit /b %ERRORLEVEL%
)
cd ..

echo.
echo Done!
