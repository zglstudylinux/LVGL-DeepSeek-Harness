# LVGL-DeepSeek-Harness - PC 一键构建脚本 (MSYS2 UCRT64)
# 用法:
#   .\build.ps1                          # 构建 demo_widgets (pc_sdl)
#   .\build.ps1 -App hello_world -Run    # 构建并运行 hello_world
#   .\build.ps1 -Clean                   # 先清空 build 目录
param(
    [string]$App  = "demo_widgets",
    [string]$Port = "pc_sdl",
    [switch]$Run,
    [switch]$Clean
)

# NOTE: do NOT set $ErrorActionPreference='Stop' here — on Windows PowerShell 5.1 a
# native command's stderr (harmless CMake warnings) would be promoted to a fatal error.
# Real failures are caught via explicit $LASTEXITCODE checks below.

# Ensure relative paths (build/, bin/) resolve against the script's own directory
Set-Location $PSScriptRoot

# --- Locate MSYS2 (override via $env:MSYS2_ROOT if needed) ---
$msys2 = if ($env:MSYS2_ROOT) { $env:MSYS2_ROOT } else { 'C:\Users\admin\msys64' }
$ucrt64 = Join-Path $msys2 'ucrt64'

# GCC spawns sub-processes (cc1/as/ld) that load runtime DLLs from ucrt64\bin
$env:PATH = "$ucrt64\bin;$env:PATH"

$cmake   = Join-Path $ucrt64 'bin\cmake.exe'
$ninja   = Join-Path $ucrt64 'bin\ninja.exe'
$gcc     = Join-Path $ucrt64 'bin\gcc.exe'
$gxx     = Join-Path $ucrt64 'bin\g++.exe'
$binDir  = Join-Path $PSScriptRoot 'bin'
$exe     = Join-Path $binDir "$App.exe"

foreach ($t in @($cmake, $ninja, $gcc, $gxx)) {
    if (-not (Test-Path $t)) { throw "Toolchain not found: $t`nSet `$env:MSYS2_ROOT to your MSYS2 install dir." }
}

if ($Clean -and (Test-Path (Join-Path $PSScriptRoot 'build'))) {
    Remove-Item (Join-Path $PSScriptRoot 'build') -Recurse -Force
}

# --- Configure (Ninja + UCRT64 SDL2) ---
# PS 5.1 quirk: unquoted "-DNAME=$var" is NOT variable-expanded — always quote these args.
$prefix = ($ucrt64 -replace '\\', '/')   # cmake wants forward slashes
$cmakeArgs = @(
    '-B', 'build',
    '-G', 'Ninja',
    '-DCMAKE_BUILD_TYPE=Debug',
    "-DCMAKE_C_COMPILER=$gcc",
    "-DCMAKE_CXX_COMPILER=$gxx",
    "-DCMAKE_PREFIX_PATH=$prefix",
    "-DCMAKE_MAKE_PROGRAM=$ninja",
    "-DAPP=$App",
    "-DPORT=$Port"
)
& $cmake @cmakeArgs
if ($LASTEXITCODE -ne 0) { throw "CMake configure failed" }

# --- Build ---
& $cmake --build build
if ($LASTEXITCODE -ne 0) { throw "Build failed" }

# --- Copy runtime DLLs next to the exe (Windows) ---
New-Item -ItemType Directory -Path $binDir -Force | Out-Null
foreach ($dll in @('SDL2.dll', 'libstdc++-6.dll', 'libgcc_s_seh-1.dll', 'libwinpthread-1.dll')) {
    $src = Join-Path $ucrt64 "bin\$dll"
    if (Test-Path $src) { Copy-Item $src $binDir -Force }
}

Write-Host "`n[OK] Built: $exe" -ForegroundColor Green

if ($Run) {
    Write-Host "Launching $App (close the window or press Ctrl+C to quit)..."
    & $exe
}
