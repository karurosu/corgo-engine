@echo off

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
echo.

echo Rebuilding CMake solutions...
echo.

echo Rebuilding build.vs2022...
mkdir build.vs2022
cd build.vs2022
cmake ..
cd ..
echo.

echo Rebuilding build.pd...
mkdir build.pd
cd build.pd
cmake .. -G "NMake Makefiles" --toolchain=c:/PlaydateSDK/C_API/buildsupport/arm.cmake
cd ..
echo.

echo Rebuilding build.pd.release...
mkdir build.pd.release
cd build.pd.release
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release --toolchain=c:/PlaydateSDK/C_API/buildsupport/arm.cmake
cd ..
echo.

echo Rebuilding build.nmake...
mkdir build.nmake
cd build.nmake
cmake .. -G "NMake Makefiles" ..

if %ERRORLEVEL% neq 0 (
    cd ..
    echo.
    echo CMake configuration failed!
    pause
    exit /b %ERRORLEVEL%
)
cd ..
echo.

echo Done!
