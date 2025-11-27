# Corgo Engine

A minimalistic ECS (Entity Component System) engine for the Panic Playdate handheld console, written in pure C.

**Status**: Early development

## Requirements

- [Playdate SDK](https://play.date/dev/) (set `PLAYDATE_SDK_PATH` environment variable)
- [Visual Studio 2022](https://visualstudio.microsoft.com/) with C/C++ development tools
- [ARM GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) (arm-none-eabi-gcc 12.2) for device builds
- [CMake](https://cmake.org/) 3.14 or higher

## Quick Start

### Setup
1. Install required tools
2. Set `PLAYDATE_SDK_PATH` environment variable to your SDK installation path
3. Run `UpdateSolutions.bat` to generate CMake build configurations

### Building
- `BuildNMake.bat` - Build simulator version
- `BuildDevice.bat` - Build device debug version
- `BuildDeviceRetail.bat` - Build device release version

### Debugging
Open `build.vs2022/corgo_engine.sln` in Visual Studio 2022 and press F5 to build and launch the Playdate Simulator.

## License

BSD 3-Clause License. See `LICENSE` for full text.
