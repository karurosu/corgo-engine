# Corgo Engine

A short-legged but complete ECS (Entity Component System) engine for the Playdate handheld console, written in pure C.
Intended to be a solid engine for writing most kinds of games, designed with flexibility in mind.

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
1. ECS Core: 90%. Need to do an optimization pass once everything else is running
2. Asset loader: Done
4. SceneGraph: Done
5. Scene management: Done
6. Engine Main loop: Mostly clean, will be updated as needed
7. Graphics: **WIP**
8. Audio: nothing yet
9. Input: Done
11. Playdate backend: minimal, just what is required for the demo
12. PC backend: nothing yet
13. Entity Streamer: nothing yet

## Quick Start (for engine development)
### Setup
1. Install required tools
2. Set `PLAYDATE_SDK_PATH` environment variable to your SDK installation path
3. Run `UpdateSolutions.bat` to generate CMake build configurations

### CMake Presets (VS Code default)
Available Cmake presets for use within VSCode (or via CLI):
- `vs2022-sim` -> Visual Studio 2022 simulator configure (`build.vs2022/`)
- `nmake-sim-devshell` -> NMake simulator configure (`build.nmake/`), requires a Visual Studio Developer shell
- `pd-debug` -> Playdate ARM debug configure (`build.pd/`), uses `NMAKE_PATH`
- `pd-release` -> Playdate ARM release configure (`build.pd.release/`), uses `NMAKE_PATH`

In order to build using the pd-* presets `NMAKE_PATH` must be set and point to VS nmake.exe, running `scripts/device-cmake.ps1` will set it for you.
All other presets can be built directly.

### Building Manually
I have created some simple batch scrips that can automate building:

- `UpdateSolutions.bat` - Update cmake configurations
- `BuildNMake.bat` - Build simulator version
- `BuildDevice.bat` - Build device debug version
- `BuildDeviceRetail.bat` - Build device release version

### Debugging
Build and run in VS Code
Open `build.vs2022/corgo_engine.sln` in Visual Studio 2022 and press F5 to build and launch the Playdate Simulator.

### Running tests
The engine has a small suite of unit tests for the core ECS and tools based on Unity Test. 
To run:
1. Build the coretest project
2. Run coretest.exe or use the CMake target

### Running Samples
Building the source will compile all the included samples, these double as tests for the engine.
Samples can be removed from the build by commenting out `CE_ENGINE_INCLUDE_SAMPLE_SCENES` in engine/config.h

## 3rd Party Credits

Convenient Containers by JacksonAllan.
MIT License
https://github.com/JacksonAllan/CC?tab=readme-ov-file

Unity Test by ThrowInTheSwitch
MIT License
https://github.com/ThrowTheSwitch/Unity?tab=readme-ov-file

printf by Eyal Rozenberg
MIT License
https://github.com/eyalroz/printf

## License

BSD 3-Clause License. See `LICENSE` for full text.
