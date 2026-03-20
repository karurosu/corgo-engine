#
#  scripts/device-cmake.ps1
#  Resolves nmake.exe and invokes cmake for device presets.
#  Copyright (c) 2026 Carlos Camacho. All rights reserved.
#
#  Usage:
#    device-cmake.ps1 -Preset pd-debug             # configure
#    device-cmake.ps1 -Preset build-pd-debug -Build # build
#
param(
    [Parameter(Mandatory)][string]$Preset,
    [switch]$Build
)

# Resolve nmake.exe: honour NMAKE_PATH if already set, otherwise use vswhere.
if (-not $env:NMAKE_PATH) {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (-not (Test-Path $vswhere)) {
        Write-Error "vswhere.exe not found at '$vswhere'. Install Visual Studio 2022."
        exit 1
    }
    $env:NMAKE_PATH = & $vswhere -latest -find "VC/Tools/MSVC/*/bin/HostX64/x64/nmake.exe" |
        Select-Object -First 1

    # Persist for future sessions (CMake Tools reads this at startup).
    [System.Environment]::SetEnvironmentVariable("NMAKE_PATH", $env:NMAKE_PATH, "User")
    Write-Host "NMAKE_PATH persisted as user environment variable. Restart VS Code for CMake Tools to pick it up."
}

if (-not $env:NMAKE_PATH) {
    Write-Error "Could not locate nmake.exe. Set NMAKE_PATH or install Visual Studio 2022."
    exit 1
}

Write-Host "Using nmake: $env:NMAKE_PATH"

if ($Build) {
    cmake --build --preset $Preset
} else {
    cmake --preset $Preset
}

exit $LASTEXITCODE
