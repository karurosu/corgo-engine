# Corgo Engine - Playdate Game Development

## Project Overview
This is an ECS (Entity Component System) engine for the Panic Playdate handheld console, written in C11.

## Copilot Context
- Session summary and latest working notes: see `.github/copilot-session.md`.
- Review that file before making build/tooling changes to stay aligned with current workflows.
- Notes should be concise, focusing on a high level description of the changes and not the full details.

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

### Third-Party Dependencies (FetchContent)
- Dependencies are fetched at configure time using CMake `FetchContent` and stored in the repo under `3rdParty/`:
    - CC (Convenient Containers): `https://github.com/JacksonAllan/CC` → `3rdParty/cc`
    - Unity (Test framework): `https://github.com/ThrowTheSwitch/Unity` → `3rdParty/unity`
- Include paths are added to all targets; Unity builds as `unity_lib` for tests. Downloads persist across builds and only re-fetch if the folder/tag changes.

### Build Configurations (4 Total)
1. **build.vs2022/** - Visual Studio 2022 solution for Windows simulator (for debugging)
2. **build.nmake/** - NMake build for simulator (CLI builds)
3. **build.pd/** - NMake build for Playdate device debug (ARM toolchain) → `corgo_engine_DEVICE.elf`
4. **build.pd.release/** - NMake build for Playdate device release (ARM toolchain) → `corgo_engine_RELEASE.elf`

### Playdate SDK Integration
**Critical**: Project requires `PLAYDATE_SDK_PATH` environment variable set. The SDK provides:
- C API headers at `$SDK/C_API/pd_api.h`
- Build support at `$SDK/C_API/buildsupport/`
     Press F7 or use `CMake: Build` to build with CMake Tools (uses `build.nmake`).
- `pdc` compiler tool for creating .pdx bundles
- Simulator at `$SDK/bin/PlaydateSimulator.exe`

### Build Artifact Names
**Game name variables** in `CMakeLists.txt`:
```cmake
PLAYDATE_GAME_NAME = corgo_engine          # Simulator builds
PLAYDATE_GAME_DEVICE = corgo_engine_DEVICE # Device debug builds

**Toolchain detection**: CMake checks `TOOLCHAIN` variable:
- `TOOLCHAIN=armgcc` + `CMAKE_BUILD_TYPE=Release` → `corgo_engine_RELEASE.elf`
- `TOOLCHAIN=armgcc` (default) → `corgo_engine_DEVICE.elf`
- Otherwise → Simulator `.dll`

    - `cmake.buildDirectory`: `${workspaceFolder}/build.nmake`
    - `cmake.sourceDirectory`: `${workspaceFolder}`
    - `cmake.configureOnOpen`: `false`
- Build via Command Palette: `CMake: Configure`, `CMake: Build`.
- Debug/Run simulator via `launch.json`: launches `${env:PLAYDATE_SDK_PATH}/bin/PlaydateSimulator.exe` with `${workspaceFolder}/corgo_engine.pdx`.

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

### Building/Running in VS Code
- Press F7 or use `CMake: Build` to build with CMake Tools (uses `build.nmake`).
- Press F5 to launch Playdate Simulator with the generated PDX at the repo root (`corgo_engine.pdx`).
- The CMake build task is used as the preLaunch task instead of batch files.

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

Note: We no longer relocate PDX folders after build; they remain at the repo root (e.g., `corgo_engine.pdx/`). Moving them in CMake interferes with `clean`.

## Code Patterns & Conventions

### Naming Conventions
- **Engine Prefix**: All engine variables, functions, types, and macros must use the `CE_` prefix (Corgo Engine).
- Examples: `CE_ECS_Context`, `CE_ECS_Init()`, `CE_CORE_DEBUG_COMPONENT`, `CE_MAX_COMPONENT_TYPES`
- This prevents namespace collisions with Playdate SDK and third-party libraries.

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

### Testing (Unity + CTest)
- Unity is integrated via `unity_lib` (from `3rdParty/unity/src/unity.c`).
- Host-only tests (excluded for `TOOLCHAIN=armgcc`).
- Test target: `coretests` (sources: `src/tests/core.c` + core engine sources).
- CTest is enabled; run tests with `ctest --output-on-failure` in `build.nmake`.
- MSVC warning C5045 is disabled for Unity and the `coretests` target.
- Use `#define CE_CORE_TEST_MODE` before including ECS headers to isolate core components only (excludes engine/game components in tests).

### Include Path Configuration
- `src/` is added to include directories for all targets (simulator, device, tests).
- Headers can be included using absolute paths from `src/`, e.g., `#include "ecs/ecs.h"`.
- Primary ECS entry point: `#include "ecs/ecs.h"` aggregates all ECS types, components, and storage.

## ECS Component System

### Component Architecture
The ECS uses an X-macro pattern for extensible component registration. Components are organized into three categories:

#### Component Types
1. **Core Components** (`src/ecs/core/core_components.h`)
   - Essential ECS infrastructure components
   - UIDs 0-10 reserved
   - Example: `CE_CORE_DEBUG_COMPONENT` (UID 0)
   - Always included in builds

2. **Engine Components** (`src/engine/components.h`)
   - Reusable engine-level components (rendering, physics, input, etc.)
   - UIDs 11-99 recommended
   - Examples: Transform, Sprite, Physics
   - Excluded in `CE_CORE_TEST_MODE`

3. **Game Components** (`src/game/components.h`)
   - Project-specific gameplay components
   - UIDs 100+ recommended
   - Examples: Health, AI, Inventory
   - Excluded in `CE_CORE_TEST_MODE`

### Component Definition Pattern
Each component requires:

**1. Header Declaration** (e.g., `src/ecs/core/core_components.h`):
```c
// Define the component data structure
typedef struct CE_Core_DebugComponent {
    bool enabled;
} CE_Core_DebugComponent;

// Add to the component descriptor list (X-macro)
#define CE_COMPONENT_DESC_CORE(X) \
    X(CE_CORE_DEBUG_COMPONENT, 0, CE_Core_DebugComponent)
```

**2. Implementation File** (e.g., `src/ecs/core/core_components.c`):
```c
#include "ecs/components.h"

// Implement init function
void CE_CORE_DEBUG_COMPONENT_init(OUT CE_Core_DebugComponent* component) {
    component->enabled = false;
}

// Implement cleanup function
void CE_CORE_DEBUG_COMPONENT_cleanup(OUT CE_Core_DebugComponent* component) {
    // Free any dynamic resources
}

// Generate component descriptor registration
CE_GENERATE_COMPONENT_IMP(CE_CORE_DEBUG_COMPONENT, 0, CE_Core_DebugComponent)
```

### Adding a New Component

**Step 1**: Choose the appropriate category (core/engine/game) and assign a unique UID.

**Step 2**: Define the struct and add to the X-macro list in the corresponding header:
```c
// In src/engine/components.h (for engine components)
typedef struct CE_Engine_HealthComponent {
    int currentHealth;
    int maxHealth;
} CE_Engine_HealthComponent;

#define CE_COMPONENT_DESC_ENGINE(X) \
    X(CE_ENGINE_HEALTH, 20, CE_Engine_HealthComponent) \
    /* other engine components... */
```

    ### CE_Id Schema
    - Kind field: bits 32–29 (4 bits) encode `CE_IdKind`.
      - 0: invalid, 1: entity reference, 2: relationship, 3: component, 4–15 reserved.
    - Entity reference fields:
      - Generation: bits 28–25 (4 bits), values 0–15.
      - Unique id: bits 16–1 (16 bits), values 0–65535.
    - Relationship fields:
      - Type: bits 24–17 (8 bits), values 0–255.
      - Unique id: bits 16–1 (16 bits).
    - Component fields:
      - Type: bits 24–17 (8 bits), values 0–255.
      - Unique id: bits 16–1 (16 bits).

    Notes:
    - Relationships no longer carry a generation value.
    - `CE_TypeId` is 8-bit.

**Step 3**: Create implementation file with init, cleanup, and descriptor generation:
```c
// In src/engine/components/health.c
    - Test target: `coretests` (sources under `src/tests/` + core engine sources).
    - Preferred run: execute the test binary directly: `build.nmake/coretests.exe`.
    - CTest may not list tests in some configs; if registered, `ctest --output-on-failure` works from `build.nmake`.
void CE_ENGINE_HEALTH_init(OUT CE_Engine_HealthComponent* component) {
    component->currentHealth = 100;
    component->maxHealth = 100;
}
    - Id schema supports up to 256 component type ids (8-bit `CE_TypeId`).
    - Current query bitmask is 64-bit; practical concurrent typed-bit queries are limited to 64 types.
    - A compile-time assertion in `src/ecs/components.h` may enforce the 64-type mask limit even if ids allow more.
    // Cleanup if needed
}

CE_GENERATE_COMPONENT_IMP(CE_ENGINE_HEALTH, 20, CE_Engine_HealthComponent)
```

**Step 4**: Rebuild. The component enum value, descriptor registration, and init/cleanup functions are automatically generated via X-macros. The `.c` file is automatically picked up by CMake's `file(GLOB ...)` patterns.

### Component Registration Details
- **Enum Generation**: `CE_ComponentTypesEnum` in `src/ecs/components.h` contains all component types.
- **Descriptor Array**: `CE_ECS_Context.componentDefinitions[]` holds static metadata (UID, type, bitmask).
- **Init Function**: `CE_ECS_Init()` populates descriptors by calling each component's `_description()` function.
- **Bitmask**: Each component gets a unique bit `(1ULL << componentType)` for efficient entity queries.
- **UID Immutability**: Component UIDs must never change after initial use (breaks serialization).

### Component Limits
- Maximum 64 component types (`CE_MAX_COMPONENT_TYPES`) to fit in a 64-bit bitmask.
- Compile-time assertion enforces this limit in `src/ecs/components.h`.

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
