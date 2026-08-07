# Verificar rutas de vcpkg y proyecto
Write-Host "=== Verificación de Rutas ===" -ForegroundColor Cyan

$paths = @(
    "C:\Users\angel\Desktop\vcpkg\installed\x64-windows\include\GL\glew.h",
    "C:\Users\angel\Desktop\vcpkg\installed\x64-windows\include\GLFW\glfw3.h",
    "C:\Users\angel\Desktop\vcpkg\installed\x64-windows\include\assimp\Importer.hpp",
    "C:\Users\angel\Desktop\vcpkg\installed\x64-windows\include\glm\glm.hpp",
    "C:\Users\angel\Desktop\vcpkg\installed\x64-windows\lib\glfw3.lib",
    "C:\Users\angel\Desktop\vcpkg\installed\x64-windows\lib\glew32.lib",
    "C:\Users\angel\Desktop\vcpkg\installed\x64-windows\lib\assimp-vc145-mt.lib"
)

foreach ($path in $paths) {
    if (Test-Path $path) {
        Write-Host "✓ $path" -ForegroundColor Green
    } else {
        Write-Host "✗ FALTA: $path" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "=== Archivos del Proyecto ===" -ForegroundColor Cyan

$project_paths = @(
    "C:\Users\angel\Desktop\Universidad\8vo\Computacion Grafica\Proyectos\Proyecto 2\Simulador_avion\Simulador_avion\Simulador_avion.vcxproj",
    "C:\Users\angel\Desktop\Universidad\8vo\Computacion Grafica\Proyectos\Proyecto 2\Simulador_avion\Simulador_avion\main.cpp",
    "C:\Users\angel\Desktop\Universidad\8vo\Computacion Grafica\Proyectos\Proyecto 2\Simulador_avion\Simulador_avion\shader.h",
    "C:\Users\angel\Desktop\Universidad\8vo\Computacion Grafica\Proyectos\Proyecto 2\Simulador_avion\Simulador_avion\shaders\vertex.glsl"
)

foreach ($path in $project_paths) {
    if (Test-Path $path) {
        Write-Host "✓ $path" -ForegroundColor Green
    } else {
        Write-Host "✗ FALTA: $path" -ForegroundColor Red
    }
}

Write-Host ""
Write-Host "=== Resultado ===" -ForegroundColor Cyan
$missing = $paths | Where-Object { -not (Test-Path $_) }
if ($missing.Count -eq 0) {
    Write-Host "✓ Todas las rutas son válidas. Intenta compilar." -ForegroundColor Green
} else {
    Write-Host "✗ Faltan $($missing.Count) archivos de vcpkg. Instala con:" -ForegroundColor Red
    Write-Host "  vcpkg install glfw3:x64-windows glew:x64-windows assimp:x64-windows glm:x64-windows"
}
