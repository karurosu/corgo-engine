# Corgo Engine - Playdate Game Development

## Project Overview
This is an ECS (Entity Component System) engine for the Panic Playdate handheld console, written in C11.

## Playdate API Documentation
The Playdate C API is accessed through a `PlaydateAPI*` pointer provided to the event handler. All functionality (graphics, system, file, sound, sprite, display, etc.) is accessed through this pointer's subsystem fields.

Official documentation: https://sdk.play.date/3.0.1/Inside%20Playdate%20with%20C.html

Key API patterns:
- Event handler receives `PDSystemEvent` (kEventInit, kEventTerminate, etc.)
- Update callback registered via `pd->system->setUpdateCallback()` - returns 1 to continue loop
- Graphics drawn via `pd->graphics->*` functions (coordinates are integers, origin top-left)
- Input accessed via `pd->system->getButtonState()` and `pd->system->getCrankAngle()`
- Sprites managed through `pd->sprite->*` functions

## Build System Architecture

### CMake-Only Build System
The project uses **CMake exclusively** via `CMakeLists.txt` which includes `$(SDK)/C_API/buildsupport/playdate_game.cmake`.

### Build Configurations (4 Total)
1. **build.vs2022/** - Visual Studio 2022 solution for Windows simulator (for debugging)
2. **build.nmake/** - NMake build for simulator (CLI builds)
3. **build.pd/** - NMake build for Playdate device debug (ARM toolchain) → `corgo_engine_DEVICE.elf`
4. **build.pd.release/** - NMake build for Playdate device release (ARM toolchain) → `corgo_engine_RELEASE.elf`

### Playdate SDK Integration
**Critical**: Project requires `PLAYDATE_SDK_PATH` environment variable set. The SDK provides:
- C API headers at `$SDK/C_API/pd_api.h`
- Build support at `$SDK/C_API/buildsupport/`
- ARM cross-compiler toolchain (arm-none-eabi-gcc 12.2)
- `pdc` compiler tool for creating .pdx bundles
- Simulator at `$SDK/bin/PlaydateSimulator.exe`

### Build Artifact Names
**Game name variables** in `CMakeLists.txt`:
```cmake
PLAYDATE_GAME_NAME = corgo_engine          # Simulator builds
PLAYDATE_GAME_DEVICE = corgo_engine_DEVICE # Device debug builds
PLAYDATE_GAME_DEVICE = corgo_engine_RELEASE # Device release builds
```

**CRITICAL**: `PLAYDATE_GAME_DEVICE` variable name must be used for device builds as it's referenced by the Playdate SDK's `playdate_game.cmake`. The value assigned to it changes based on `CMAKE_BUILD_TYPE` (Debug → `corgo_engine_DEVICE`, Release → `corgo_engine_RELEASE`).

**Toolchain detection**: CMake checks `TOOLCHAIN` variable:
- `TOOLCHAIN=armgcc` + `CMAKE_BUILD_TYPE=Release` → `corgo_engine_RELEASE.elf`
- `TOOLCHAIN=armgcc` (default) → `corgo_engine_DEVICE.elf`
- Otherwise → Simulator `.dll`

## Key Developer Workflows

### Regenerating CMake Solutions
```batch
UpdateSolutions.bat
```
This regenerates all 4 build configurations. Requires Visual Studio Developer Command Prompt environment.

### Building from Command Line
```batch
BuildNMake.bat           # Build simulator (NMake)
BuildDevice.bat          # Build device debug
BuildDeviceRetail.bat    # Build device release
```
All batch files auto-configure VS dev environment and pause only on errors.

### Debugging in Visual Studio
1. Open `build.vs2022/corgo_engine.sln` in Visual Studio 2022
2. Build and run (F5) - pre-configured to launch Playdate Simulator

The VS project is pre-configured with:
- **Command**: `C:\PlaydateSDK\bin\PlaydateSimulator.exe`
- **Arguments**: `"D:\git\corgo-engine\corgo_engine.pdx"`

### Output Artifacts
- **Simulator**: `Source/pdex.dll` → packaged into `corgo_engine.pdx/`
- **Device Debug**: `corgo_engine_DEVICE.elf` → packaged into `corgo_engine_DEVICE.pdx/`
- **Device Release**: `corgo_engine_RELEASE.elf` → packaged into `corgo_engine_RELEASE.pdx/`
- **PDX metadata**: `Source/pdxinfo` contains game name, author, bundle ID

## Code Patterns & Conventions

### Playdate C API Usage Pattern
All Playdate functionality accessed through `PlaydateAPI*` pointer:
```c
int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg) {
    if (event == kEventInit) {
        // Setup: load fonts, set update callback
        pd->graphics->loadFont(fontpath, &err);
        pd->system->setUpdateCallback(update, pd);
    }
}

static int update(void* userdata) {
    PlaydateAPI* pd = userdata;
    pd->graphics->clear(kColorWhite);
    pd->graphics->drawText(...);
    return 1; // Continue game loop
}
```

### Display Constants
- Screen dimensions: `LCD_COLUMNS` (400) × `LCD_ROWS` (240)

### Source Organization
- To add source files: Update `add_executable` or `add_library` calls in `CMakeLists.txt`
- Example: `add_executable(${PLAYDATE_GAME_DEVICE} src/main.c src/engine/ecs.c src/engine/systems.c)`

### Compiler Flags
- **Device builds** (ARM): `-Wdouble-promotion -Werror` (fail on warnings, warn on float→double promotion)
- **Simulator builds** (Windows): `-WX` (treat warnings as errors)

## Project Metadata
**From `Source/pdxinfo`:**
- Name: Corgo Engine
- Author: Carlos Camacho
- Bundle ID: com.karurosu.corgoengine
- Description: A small ECS based engine for the playdate

## Platform-Specific Notes

### Windows Development
- Requires ARM GNU Toolchain for device builds
- Visual Studio 2022 used for simulator development
- NMake used for command-line builds
- PowerShell preferred over bash for scripting

## Important Constraints
- **Pure C mode**: Setting update callback in `kEventInit` prevents Lua execution
- **No hot reload**: Changes require full rebuild + simulator restart
