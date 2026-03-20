# Corgo Engine - Playdate Game Development

## Project Overview
This is an ECS (Entity Component System) engine for the Panic Playdate handheld console, written in C11.

## Copilot Context
- This file contains all the context needed for the project.
- When making significant changes, update relevant sections to keep documentation accurate.

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
PLAYDATE_GAME_DEVICE = corgo_engine_DEVICE # Device debug builds (or corgo_engine_RELEASE for release)
```

**Toolchain detection**: CMake checks `TOOLCHAIN` variable and `CMAKE_BUILD_TYPE`:
- `TOOLCHAIN=armgcc` + `CMAKE_BUILD_TYPE=Release` → `corgo_engine_RELEASE.elf`
- `TOOLCHAIN=armgcc` (default) → `corgo_engine_DEVICE.elf`
- Otherwise → Simulator `.dll`

**VS Code CMake integration** (via `.vscode/settings.json`):
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
BuildDeviceRelease.bat   # Build device release
```
All batch files auto-configure VS dev environment and pause only on errors.

### Building/Running in VS Code
- Press F7 or use `CMake: Build` to build with CMake Tools (uses `build.nmake`).
- Press F5 to launch Playdate Simulator with the generated PDX at the repo root (`corgo_engine.pdx`).
- The CMake build task is used as the preLaunch task instead of batch files.

### CMake Presets Workflow (Preferred for Tooling)
This repository includes `CMakePresets.json` with presets matching the existing 4 build trees:
- `vs2022-sim` -> `build.vs2022/`
- `sim-compat` -> compatibility alias to `vs2022-sim` (for tooling reliability)
- `nmake-sim-devshell` -> `build.nmake/` (requires VS developer environment)
- `pd-debug` -> `build.pd/` (requires `NMAKE_PATH` to point to `nmake.exe` when not running in a developer shell)
- `pd-release` -> `build.pd.release/` (requires `NMAKE_PATH` to point to `nmake.exe` when not running in a developer shell)

Important for automation/agents on Windows:
- NMake presets still require Visual Studio developer environment initialization.
- Device presets can avoid a hardcoded repo path by reading `nmake.exe` from the `NMAKE_PATH` environment variable.
- `scripts/device-cmake.ps1` is optional and intended for device preset workflows only; it may persist `NMAKE_PATH` to the user environment to help CMake Tools discover `nmake.exe`.
- If a tool cannot configure directly, run `UpdateSolutions.bat` first (or `cmd /c "UpdateSolutions.bat <nul"` for non-interactive runs), then build with CMake Tools or presets.
- Do not modify `CMakeLists.txt` to work around missing shell environment; prefer preset selection plus proper environment setup.

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

### C Style Guide

#### Function Naming
All functions follow the pattern: `CE_<Module>_<Action>` or `CE_<Module>_<Submodule>_<Action>`

**Rules:**
- Module/submodule names use PascalCase: `CE_ECS_Init()`, `CE_Entity_AddComponent()`, `CE_Bitset_init()`
- Actions are descriptive verbs: `Init`, `Cleanup`, `Create`, `Destroy`, `Add`, `Remove`, `Get`, `Find`, `Set`, `Toggle`
- Internal utility functions may use lowercase: `CE_Bitset_init()` vs `CE_ECS_Init()`
- Boolean queries use `is` or `has` prefix: `CE_Bitset_isBitSet()`, `CE_Entity_HasComponent()`

**Examples:**
```c
CE_Result CE_ECS_Init(INOUT CE_ECS_Context* context, OUT_OPT CE_ERROR_CODE* errorCode);
CE_Result CE_Entity_AddComponent(INOUT CE_ECS_Context* context, IN CE_Id entity, ...);
CE_Result CE_Bitset_setBit(INOUT CE_Bitset* bitset, IN size_t index);
bool CE_Bitset_isBitSet(IN const CE_Bitset* bitset, IN size_t index);
```

#### Parameter Annotations
Use SAL-like annotations to document parameter intent (supported by ARM compiler and static analysis):

- **`IN`** - Input parameter (read-only)
- **`OUT`** - Output parameter (write-only, must be initialized by function)
- **`INOUT`** - Input/output parameter (read and write)
- **`IN_OPT`** - Optional input parameter (can be NULL)
- **`OUT_OPT`** - Optional output parameter (can be NULL)
- **`INOUT_OPT`** - Optional input/output parameter (can be NULL)

**Guidelines:**
- Place annotations before the type: `IN CE_Id entity`, `OUT CE_Id* outId`
- Use `const` qualifiers in addition to `IN` for extra safety: `IN const CE_Bitset* bitset`
- Optional parameters (especially error codes) should use `_OPT` suffixes
- Always annotate pointer parameters to clarify ownership and mutation

**Example:**
```c
CE_Result CE_Entity_GetComponent(
    INOUT CE_ECS_Context* context,      // Context is read and modified
    IN CE_Id entity,                     // Entity ID is read-only
    IN CE_Id componentId,                // Component ID is read-only
    OUT void **componentData,            // Filled with component pointer
    OUT_OPT CE_ERROR_CODE* errorCode     // Optional detailed error info
);
```

#### Docstring Style
Use Javadoc/Doxygen-style documentation comments for all public APIs:

**Structure:**
1. Opening `/**` on its own line
2. `@brief` one-line summary
3. Blank line (using ` * `)
4. Detailed description (optional, multiple lines)
5. Blank line (using ` * `)
6. `@param[direction]` for each parameter (direction: `in`, `out`, or `in,out`)
7. Blank line (using ` * `)
8. `@return` describing the return value
9. Closing `*/`

**Example:**
```c
/**
 * @brief Initialize a bitset with the given size.
 * 
 * Sets up a bitset to hold the specified number of bits, all initially cleared.
 * The bitset must not be already initialized.
 * 
 * @param[in,out] bitset The bitset to initialize.
 * @param[in] size The number of bits the bitset should hold (max: CE_BITSET_MAX_BITS).
 * 
 * @return CE_OK on success, CE_ERROR on failure.
 */
CE_Result CE_Bitset_init(OUT CE_Bitset* bitset, IN size_t size);
```

**Guidelines:**
- Keep `@brief` to one sentence, present tense: "Initialize...", "Set...", "Get..."
- Include important context in the detailed description (preconditions, side effects)
- Match `@param` direction to parameter annotations: `[in]` for `IN`, `[out]` for `OUT`, `[in,out]` for `INOUT`
- Describe what each parameter does, not just its type
- `@return` should describe both success and failure cases

#### Error Handling with CE_Result and CE_ERROR_CODE

**CE_Result:**
- Type: `int`
- Success: `CE_OK` (value: 0)
- Failure: `CE_ERROR` (value: -1)
- All functions that can fail must return `CE_Result`

**CE_ERROR_CODE:**
- Enum type providing detailed error information
- Defined using X-macro pattern in `src/utils/error.h`
- Organized by subsystem (0-9: general, 10-19: storage, 30-39: entity, etc.)
- Use `CE_ERROR_CODE_NONE` (0) to indicate no error

**Function Signature Pattern:**
```c
CE_Result FunctionName(
    INOUT CE_ECS_Context* context,
    /* ...other parameters... */
    OUT_OPT CE_ERROR_CODE* errorCode  // Always last parameter
);
```

**Implementation Pattern:**
```c
CE_Result CE_Entity_AddComponent(..., OUT_OPT CE_ERROR_CODE* errorCode) {
    // Validate inputs
    if (!context) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_INVALID_ENTITY_ID);
        return CE_ERROR;
    }
    
    // Perform operation
    if (operation_failed) {
        CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_MAX_ENTITIES_REACHED);
        return CE_ERROR;
    }
    
    // Success
    CE_SET_ERROR_CODE(errorCode, CE_ERROR_CODE_NONE);
    return CE_OK;
}
```

**Usage Pattern:**
```c
CE_ERROR_CODE errorCode;
CE_Result result = CE_Entity_AddComponent(context, entity, type, &id, NULL, &errorCode);
if (result != CE_OK) {
    const char* errorMsg = CE_GetErrorMessage(errorCode);
    pd->system->logToConsole("Failed to add component: %s", errorMsg);
    return CE_ERROR;
}
```

**Guidelines:**
- Functions returning `CE_Result` should always have `OUT_OPT CE_ERROR_CODE* errorCode` as the last parameter
- Use `CE_SET_ERROR_CODE(errorCode, code)` macro to safely set error codes (handles NULL checks)
- Always set error code to `CE_ERROR_CODE_NONE` on success
- Return `CE_ERROR` immediately after setting error code on failure
- Choose the most specific error code available
- When adding new error codes, follow the subsystem numbering scheme in `error.h`

## Source code conventions ##
For new or modified source files, follow these conventions.

Top-of-file comment block format:
```c
//
//  <path to file>/<source file>.h/.c
//  <One line description>
//  Copyright (c) <year> Carlos Camacho. All rights reserved.
//
```

Header guards format:
```c
#ifndef <UPPERCASE_PATH>_<UPPERCASE_FILE>_H
#define <UPPERCASE_PATH>_<UPPERCASE_FILE>_H

<code>

#endif // <UPPERCASE_PATH>_<UPPERCASE_FILE>_H
```

- Header files should include `#include "ecs/types.h"`.
- Header files should not include `#include "ecs/ecs.h"` or `#include "engine/corgo.h"`.
- `#include "ecs/ecs.h"` is for `src/ecs/core/*.c` files only.
- All non-core source files should use `#include "engine/corgo.h"` as the primary engine include.
- Avoid including component implementation headers directly in `.c` files when the symbol is already available through `engine/corgo.h`.

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

### Testing (Unity + CTest)
- Unity is integrated via `unity_lib` (from `3rdParty/unity/src/unity.c`).
- Host-only tests (excluded for `TOOLCHAIN=armgcc`).
- Test target: `coretests` (sources: `src/tests/core.c` + core engine sources).
- CTest is enabled; run tests with `ctest --output-on-failure` in `build.nmake`.
- MSVC warning C5045 is disabled for Unity and the `coretests` target.
- Use `#define CE_CORE_TEST_MODE` before including ECS headers to isolate core components only (excludes engine/game components in tests).

### Include Path Configuration
- `src/` is added to include directories for all targets (simulator, device, tests).
- Headers can be included using absolute paths from `src/`, e.g., `#include "engine/corgo.h"`.
- Primary include for non-core files: `#include "engine/corgo.h"`.
- Core-only include for ECS internals (`src/ecs/core/*.c`): `#include "ecs/ecs.h"`.

## Asset Loader Declarations

Asset loaders are declared through X-macros and auto-generated prototypes.

### Declaration Flow
1. Add custom asset enum entries in `src/game/assets.h` via `CE_ASSET_TYPES_GAME(X)`.
2. Declare loader metadata in `src/game/assets.h` via `CE_ASSET_LOADERS_GAME(X)`.
3. `src/engine/assets.h` merges engine and game loader lists using `CE_ASSET_LOADERS(X)`.
4. `src/engine/core/asset_loader.h` expands that list and auto-declares:
   - `type##_ptr_t`
   - `type##_load_params_t`
   - `CE_Engine_LoadAsset_<type>(...)`
   - `CE_Engine_FreeAsset_<type>(...)`

### Required Macro Entry Shape
Each loader entry must follow:
```c
X(<asset type>, <pointer base type>, <load params type>)
```

- `<asset type>` must be a value from `CE_ASSET_TYPES` (for game assets, add it in `CE_ASSET_TYPES_GAME`).
- `<pointer base type>` is the non-pointer type name (for example, `LCDBitmap`, not `LCDBitmap*`).
- `<load params type>` is a struct/typedef passed to the load function, or `CE_Asset_LoadParams_None` when unused.

### Example (Game Asset)
```c
// src/game/assets.h
typedef struct CE_Game_SpriteSheetLoadParams {
    int m_frameWidth;
    int m_frameHeight;
} CE_Game_SpriteSheetLoadParams;

#define CE_ASSET_TYPES_GAME(X) \
    X(CE_ASSET_TYPE_SPRITESHEET)

#define CE_ASSET_LOADERS_GAME(X) \
    X(CE_ASSET_TYPE_SPRITESHEET, LCDBitmapTable, CE_Game_SpriteSheetLoadParams)
```

This generates declarations equivalent to:
```c
typedef LCDBitmapTable* CE_ASSET_TYPE_SPRITESHEET_ptr_t;
typedef CE_Game_SpriteSheetLoadParams CE_ASSET_TYPE_SPRITESHEET_load_params_t;

CE_ASSET_TYPE_SPRITESHEET_ptr_t CE_Engine_LoadAsset_CE_ASSET_TYPE_SPRITESHEET(
    const char* assetPath,
    const CE_ASSET_TYPE_SPRITESHEET_load_params_t* loadParams
);

CE_Result CE_Engine_FreeAsset_CE_ASSET_TYPE_SPRITESHEET(
    CE_ASSET_TYPE_SPRITESHEET_ptr_t asset
);
```

### Implementation Macros
Implement the loaders in a `.c` file (typically under `src/engine/asset_loaders/` or game-specific source) using:
- `CE_DEFINE_ASSET_LOADER_LOAD_FUNCTION(<asset type>)`
- `CE_DEFINE_ASSET_LOADER_FREE_FUNCTION(<asset type>)`

Use `CE_GENERATE_ASSET_LOADER_STUB(<asset type>)` only for temporary unimplemented loaders.

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
    bool m_enabled;
} CE_Core_DebugComponent;

// Add to the component descriptor list (X-macro)
// Parameters: (name, uid, storage_type, initial_capacity)
#define CE_COMPONENT_DESC_CORE(X) \
    X(CE_CORE_DEBUG_COMPONENT, 0, CE_Core_DebugComponent, CE_DEFAULT_COMPONENT_CAPACITY)
```

**2. Implementation File** (e.g., `src/ecs/core/components/debug_component.c`):
```c
#include "ecs/ecs.h"

// Implement init function using the macro
CE_DEFINE_COMPONENT_INIT(CE_CORE_DEBUG_COMPONENT)
{
    component->m_enabled = false;
    return CE_OK;
}

// Implement cleanup function using the macro
CE_DEFINE_COMPONENT_CLEANUP(CE_CORE_DEBUG_COMPONENT)
{
    // Free any dynamic resources if needed
    return CE_OK;
}

// Do not call CE_GENERATE_COMPONENT_IMP here.
// Wrappers/description are generated centrally from src/ecs/core/component_registry.c.
```

### Adding a New Component

**Step 1**: Choose the appropriate category (core/engine/game) and assign a unique UID.

**Step 2**: Define the struct and add to the X-macro list in the corresponding header:
```c
// In src/engine/components.h (for engine components)
typedef struct CE_Engine_HealthComponent {
    int m_currentHealth;
    int m_maxHealth;
} CE_Engine_HealthComponent;

// Add to the X-macro with 4 parameters: (name, uid, storage_type, initial_capacity)
#define CE_COMPONENT_DESC_ENGINE(X) \
    X(CE_ENGINE_HEALTH, 20, CE_Engine_HealthComponent, 32) \
    /* other engine components... */

**Step 3**: Create implementation file with init and cleanup only:
```c
// In src/engine/components/health.c
#include "engine/corgo.h"

CE_DEFINE_COMPONENT_INIT(CE_ENGINE_HEALTH)
{
    component->m_currentHealth = 100;
    component->m_maxHealth = 100;
    return CE_OK;
}

CE_DEFINE_COMPONENT_CLEANUP(CE_ENGINE_HEALTH)
{
    // Cleanup if needed
    return CE_OK;
}

```

**Step 4**: Rebuild. The component enum value comes from the X-macro lists, and wrappers/description functions are generated centrally in `src/ecs/core/component_registry.c`. The `.c` file is automatically picked up by CMake's `file(GLOB ...)` patterns.

### Component Registration Details
- **Enum Generation**: `CE_ComponentTypesEnum` in `src/ecs/components.h` contains all component types.
- **Descriptor Array**: `CE_ECS_Context.componentDefinitions[]` holds static metadata (UID, type, bitmask).
- **Init Function**: `CE_ECS_Init()` populates descriptors by calling each component's `_description()` function.
- **Bitmask**: Each component gets a unique bit `(1ULL << componentType)` for efficient entity queries.
- **UID Immutability**: Component UIDs must never change after initial use (breaks serialization).

### Component Limits
- Maximum 255 component types (`CE_MAX_COMPONENT_TYPES`) due to 8-bit type field in CE_Id.
- Value 255 is reserved for invalid type, so 0-254 are valid component types.
- Compile-time assertion enforces this limit in `src/ecs/components.h`.

## ECS Global Component System

### What are Global Components?
Global components are singleton components stored once per ECS context, not attached to individual entities. They're ideal for:
- Game-wide state (camera, display settings, asset caches)
- Scene management data (scene graph, active scene)
- Engine services (input manager, audio system)

Unlike regular components, global components:
- Have no UID field (not serializable)
- Cannot be attached to entities
- Are accessed directly via macros, not entity queries
- Have only 2 parameters in X-macro: `(name, storage_type)`

### Global Component Definition Pattern

**1. Header Declaration** (e.g., `src/ecs/core/core_components.h`):
```c
// Define the global component data structure
typedef struct CE_Core_GlobalDebugComponent {
    bool m_enabled;
} CE_Core_GlobalDebugComponent;

// Add to the global component descriptor list (X-macro)
// Parameters: (name, storage_type)
#define CE_GLOBAL_COMPONENT_DESC_CORE(X) \
    X(CE_CORE_GLOBAL_DEBUG_COMPONENT, CE_Core_GlobalDebugComponent)
```

**2. Implementation File** (e.g., `src/ecs/core/components/debug_component.c`):
```c
#include "ecs/ecs.h"

// Implement init function using the macro
CE_DEFINE_GLOBAL_COMPONENT_INIT(CE_CORE_GLOBAL_DEBUG_COMPONENT)
{
    component->m_enabled = false;
    return CE_OK;
}

// Implement cleanup function using the macro
CE_DEFINE_GLOBAL_COMPONENT_CLEANUP(CE_CORE_GLOBAL_DEBUG_COMPONENT)
{
    // Free any dynamic resources if needed
    return CE_OK;
}
```

**Important**: Global components do NOT use `CE_GENERATE_COMPONENT_IMP`. The init/cleanup functions are registered differently by the ECS.

### Accessing Global Components

**In Systems:**
```c
CE_START_GLOBAL_SYSTEM_IMPLEMENTATION(CE_MY_GLOBAL_SYSTEM)
{
    // Access global component with automatic type casting
    CE_ECS_AccessGlobalComponentToVariable(context, CE_CORE_GLOBAL_DEBUG_COMPONENT, globalDebug);
    if (!globalDebug) {
        CE_Error("Failed to access global debug component");
        return CE_ERROR;
    }
    
    // Use the global component
    globalDebug->m_enabled = true;
}
CE_END_GLOBAL_SYSTEM_IMPLEMENTATION
```

**In Game Code:**
```c
// Manual access with explicit cast
CE_Core_GlobalDebugComponent* debug = CE_ECS_AccessGlobalComponent(context, CE_CORE_GLOBAL_DEBUG_COMPONENT);
```

### Adding a New Global Component

**Step 1**: Define the struct and add to the X-macro list (2 parameters: name, storage_type):
```c
// In src/engine/components.h
typedef struct CE_Engine_InputManagerComponent {
    uint32_t m_currentButtons;
    uint32_t m_previousButtons;
} CE_Engine_InputManagerComponent;

#define CE_GLOBAL_COMPONENT_DESC_ENGINE(X) \
    X(CE_ENGINE_INPUT_MANAGER, CE_Engine_InputManagerComponent) \
    /* other global components... */
```

**Step 2**: Create implementation file with init/cleanup:
```c
// In src/engine/components/input_manager.c
#include "engine/corgo.h"

CE_DEFINE_GLOBAL_COMPONENT_INIT(CE_ENGINE_INPUT_MANAGER)
{
    component->m_currentButtons = 0;
    component->m_previousButtons = 0;
    return CE_OK;
}

CE_DEFINE_GLOBAL_COMPONENT_CLEANUP(CE_ENGINE_INPUT_MANAGER)
{
    return CE_OK;
}
```

**Step 3**: Rebuild. Global components are automatically registered without needing `CE_GENERATE_COMPONENT_IMP`.

## ECS System Architecture

### System Types
Systems process entities with specific components. The engine supports two types:

1. **Regular Systems** - Process individual entities that match component requirements
2. **Global Systems** - Run once per tick, access global components and context

### System Execution Model

Systems are organized by three metadata properties:

**Run Order** (`CE_ECS_SYSTEM_RUN_ORDER`):
- `CE_ECS_SYSTEM_RUN_ORDER_AUTO` - Standard systems, run in registration order
- `CE_ECS_SYSTEM_RUN_ORDER_SCENETREE` - Systems run in scene graph traversal order
- `CE_ECS_SYSTEM_RUN_ORDER_RENDER` - Rendering systems, run in Z-order

**Run Phase** (`CE_ECS_SYSTEM_RUN_PHASE`):
- `CE_ECS_SYSTEM_RUN_PHASE_EARLY` - Pre-processing (input, state updates)
- `CE_ECS_SYSTEM_RUN_PHASE_DEFAULT` - Main logic (gameplay, AI)
- `CE_ECS_SYSTEM_RUN_PHASE_LATE` - Post-processing (debug, cleanup)

**Run Frequency** (`CE_ECS_SYSTEM_RUN_FREQUENCY`):
- `CE_ECS_SYSTEM_RUN_FREQUENCY_DISPLAY` - Every frame (30 FPS on Playdate)
- `CE_ECS_SYSTEM_RUN_FREQUENCY_HALF_DISPLAY` - Every other frame (15 FPS)
- `CE_ECS_SYSTEM_RUN_FREQUENCY_ONCE_PER_SECOND` - Once per second (1 Hz)

### Regular System Definition Pattern

**1. Header Declaration** (e.g., `src/engine/systems.h`):
```c
// Define component dependencies macro
#define CE_TEXT_LABEL_SYSTEM_DEPENDENCIES \
    REQUIRE_COMPONENT(CE_TEXT_LABEL_COMPONENT, textLabelComponent)\
    REQUIRE_COMPONENT(CE_TRANSFORM_COMPONENT, transformComponent)

// Add to the system descriptor list (X-macro)
// Parameters: (name, run_order, run_phase, run_frequency, dependencies)
#define CE_SYSTEM_DESC_ENGINE(X) \
    X(CE_TEXT_LABEL_RENDERER, CE_ECS_SYSTEM_RUN_ORDER_RENDER, CE_ECS_SYSTEM_RUN_PHASE_DEFAULT, CE_ECS_SYSTEM_RUN_FREQUENCY_DISPLAY, CE_TEXT_LABEL_SYSTEM_DEPENDENCIES)
```

**2. Implementation File** (e.g., `src/engine/systems/text_label.c`):
```c
#include "engine/corgo.h"

CE_START_SYSTEM_IMPLEMENTATION(CE_TEXT_LABEL_RENDERER, CE_TEXT_LABEL_SYSTEM_DEPENDENCIES)
{
    // Dependencies are automatically loaded as local variables by the macro:
    // - textLabelComponent: CE_TextLabelComponent*
    // - textLabelComponent_Id: CE_Id
    // - transformComponent: CE_TransformComponent*
    // - transformComponent_Id: CE_Id
    
    // Implement system logic
    if (textLabelComponent->m_text[0] != '\0') {
        // Draw text at transform position
        DrawText(textLabelComponent->m_text, transformComponent->m_x, transformComponent->m_y);
    }
}
CE_END_SYSTEM_IMPLEMENTATION
```

**IMPORTANT**: The dependency macro variables are automatically created:
- Component pointer: `componentName` (e.g., `textLabelComponent`)
- Component ID: `componentName_Id` (e.g., `textLabelComponent_Id`)
- Relationships: `relationshipName_Id` (e.g., `parent_Id`)

### Global System Definition Pattern

**1. Header Declaration**:
```c
// Global systems have simpler parameters: (name, run_phase, run_frequency)
#define CE_GLOBAL_SYSTEM_DESC_ENGINE(X) \
    X(CE_ENGINE_INPUT_SYSTEM, CE_ECS_SYSTEM_RUN_PHASE_EARLY, CE_ECS_SYSTEM_RUN_FREQUENCY_DISPLAY)
```

**2. Implementation File**:
```c
#include "engine/corgo.h"

CE_START_GLOBAL_SYSTEM_IMPLEMENTATION(CE_ENGINE_INPUT_SYSTEM)
{
    // Access global components
    CE_ECS_AccessGlobalComponentToVariable(context, CE_ENGINE_INPUT_MANAGER, inputManager);
    
    // Update global state
    inputManager->m_previousButtons = inputManager->m_currentButtons;
    inputManager->m_currentButtons = GetCurrentButtons();
}
CE_END_GLOBAL_SYSTEM_IMPLEMENTATION
```

### System with Relationship Requirements

Systems can require entities to have specific relationships:
```c
#define CE_PARENT_CHILD_SYSTEM_DEPENDENCIES \
    REQUIRE_COMPONENT(CE_TRANSFORM_COMPONENT, transformComponent)\
    REQUIRE_RELATIONSHIP(CE_RELATIONSHIP_PARENT, parent)

CE_START_SYSTEM_IMPLEMENTATION(CE_PARENT_CHILD_SYSTEM, CE_PARENT_CHILD_SYSTEM_DEPENDENCIES)
{
    // transformComponent is available as CE_TransformComponent*
    // parent_Id is available as CE_Id
    
    // Get parent entity's transform
    CE_Id parentTransformId;
    CE_TransformComponent* parentTransform;
    if (CE_Entity_FindFirstComponent(context, parent_Id, CE_TRANSFORM_COMPONENT, 
                                      &parentTransformId, (void**)&parentTransform, errorCode) == CE_OK) {
        // Apply parent's transform to child
        transformComponent->m_x += parentTransform->m_x;
        transformComponent->m_y += parentTransform->m_y;
    }
}
CE_END_SYSTEM_IMPLEMENTATION
```

### Adding a New System

**Step 1**: Define dependencies and add to X-macro:
```c
// In src/game/systems.h
#define CE_HEALTH_SYSTEM_DEPENDENCIES \
    REQUIRE_COMPONENT(CE_GAME_HEALTH_COMPONENT, healthComponent)

#define CE_SYSTEM_DESC_GAME(X) \
    X(CE_GAME_HEALTH_SYSTEM, CE_ECS_SYSTEM_RUN_ORDER_AUTO, CE_ECS_SYSTEM_RUN_PHASE_DEFAULT, CE_ECS_SYSTEM_RUN_FREQUENCY_DISPLAY, CE_HEALTH_SYSTEM_DEPENDENCIES)
```

**Step 2**: Create implementation file:
```c
// In src/game/systems/health.c
#include "engine/corgo.h"

CE_START_SYSTEM_IMPLEMENTATION(CE_GAME_HEALTH_SYSTEM, CE_HEALTH_SYSTEM_DEPENDENCIES)
{
    // healthComponent is automatically available
    if (healthComponent->m_currentHealth <= 0) {
        // Entity is dead, handle it
        CE_Debug("Entity %u has died", entity);
        CE_ECS_DestroyEntity(context, entity, errorCode);
    }
}
CE_END_SYSTEM_IMPLEMENTATION
```

**Step 3**: Rebuild. Systems are automatically registered via X-macros.

### System Registration Details
- Systems are registered in `CE_ECS_Init()` by calling `_description()` functions
- Systems are cached in `CE_ECS_SystemRuntimeData` by run order/frequency/phase for fast iteration
- Entity-component bitsets are compared against system requirement bitsets for efficient filtering
- Systems only run on entities that match ALL required components and relationships

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
