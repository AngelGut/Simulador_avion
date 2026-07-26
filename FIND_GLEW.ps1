#!/usr/bin/env pwsh
# Buscar dónde está instalado GLEW en vcpkg

Write-Host "=== Buscando GLEW en vcpkg ===" -ForegroundColor Cyan

$vcpkg_root = "C:\Users\angel\Desktop\vcpkg"

# Verificar si vcpkg existe
if (-not (Test-Path $vcpkg_root)) {
    Write-Host "✗ vcpkg no encontrado en: $vcpkg_root" -ForegroundColor Red
    exit 1
}

Write-Host "✓ vcpkg encontrado: $vcpkg_root" -ForegroundColor Green

# Rutas posibles de GLEW
$possible_paths = @(
    "$vcpkg_root\installed\x64-windows\include\GL\glew.h",
    "$vcpkg_root\installed\x64-windows\lib\glew32.lib",
    "$vcpkg_root\packages\glew_x64-windows",
    "$vcpkg_root\packages\glew_x64-windows-static"
)

Write-Host ""
Write-Host "Verificando ubicaciones de GLEW:" -ForegroundColor Cyan

$found = $false
foreach ($path in $possible_paths) {
    if (Test-Path $path) {
        Write-Host "✓ ENCONTRADO: $path" -ForegroundColor Green
        $found = $true
    } else {
        Write-Host "✗ NO encontrado: $path" -ForegroundColor Gray
    }
}

if (-not $found) {
    Write-Host ""
    Write-Host "✗ GLEW NO está instalado en vcpkg" -ForegroundColor Red
    Write-Host ""
    Write-Host "Para instalarlo, ejecuta:" -ForegroundColor Yellow
    Write-Host "  vcpkg install glew:x64-windows" -ForegroundColor White
    exit 1
}

Write-Host ""
Write-Host "=== Listado completo de GLEW en vcpkg ===" -ForegroundColor Cyan
Write-Host ""

# Mostrar contenido de GL/
$gl_path = "$vcpkg_root\installed\x64-windows\include\GL"
if (Test-Path $gl_path) {
    Write-Host "Contenido de GL/:" -ForegroundColor Green
    ls $gl_path | Select-Object Name
}

# Mostrar librerías GLEW
Write-Host ""
$lib_path = "$vcpkg_root\installed\x64-windows\lib"
if (Test-Path $lib_path) {
    Write-Host "Librerías GLEW en lib/:" -ForegroundColor Green
    ls $lib_path | Where-Object { $_.Name -like "*glew*" } | Select-Object Name
}

Write-Host ""
Write-Host "✓ GLEW está correctamente instalado" -ForegroundColor Green
