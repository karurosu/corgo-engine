# Corgo Engine

A minimalist ECS (Entity Component System) engine for the Panic Playdate handheld console, written in pure C.

**Status**: Early development see below for details.

I am writing a devblog describing the design and development of the engine, you can read it here: https://medium.com/@mrkaru

## Requirements

- [Playdate SDK](https://play.date/dev/) (set `PLAYDATE_SDK_PATH` environment variable)
- [Visual Studio 2022](https://visualstudio.microsoft.com/) with C/C++ development tools
- [ARM GNU Toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) (arm-none-eabi-gcc 12.2) for device builds
- [CMake](https://cmake.org/) 3.14 or higher

## Demo
See the early version running in the simulator here: 
https://github.com/karurosu/corgo-engine/blob/main/corgo_demo.gif?raw=true

## Current state
1. ECS Core: 80%. Entities, Component and Systems can be defined and tick. Still missing support for scenegraph ordering, optimizations and some features.
2. Engine core: just a demo. Hardcoded to run, currently in active development.
3. SceneGraph and scripting: nothing yet.
4. Graphics: nothing yet.
5. Audio: nothing yet.
6. Input: nothing yet.
7. Asset loader: nothing yet
8. Playdate backend: minimal, just what is required for the demo.
9. PC backend: nothing yet.

## Quick Start (for engine development)
### Setup
1. Install required tools
2. Set `PLAYDATE_SDK_PATH` environment variable to your SDK installation path
3. Run `UpdateSolutions.bat` to generate CMake build configurations

### Building
- `BuildNMake.bat` - Build simulator version
- `BuildDevice.bat` - Build device debug version
- `BuildDeviceRetail.bat` - Build device release version

### Debugging
Build and run in VS Code
Open `build.vs2022/corgo_engine.sln` in Visual Studio 2022 and press F5 to build and launch the Playdate Simulator.

### Running tests
Build the coretest project
Run coretest.exe or use the CMake target

## 3rd Party Credits

Convenient Containers by JacksonAllan.
MIT License
https://github.com/JacksonAllan/CC?tab=readme-ov-file

Unity Test by ThrowInTheSwitch
MIT License
https://github.com/ThrowTheSwitch/Unity?tab=readme-ov-file

## License

BSD 3-Clause License. See `LICENSE` for full text.
