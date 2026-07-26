cmake_minimum_required(VERSION 3.10)
project(Simulador_avion)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ============================================================
# BUSCAR PAQUETES
# ============================================================

find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(assimp REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

# ============================================================
# ARCHIVOS FUENTE
# ============================================================

set(SOURCES
    Simulador_avion/main.cpp
    Simulador_avion/renderer.cpp
    Simulador_avion/model_loader.cpp
    Simulador_avion/shader.cpp
    Simulador_avion/geometry.cpp
    Simulador_avion/layer_manager.cpp
    Simulador_avion/utils.cpp
)

set(HEADERS
    Simulador_avion/renderer.h
    Simulador_avion/model_loader.h
    Simulador_avion/shader.h
    Simulador_avion/geometry.h
    Simulador_avion/layer_manager.h
    Simulador_avion/utils.h
    Simulador_avion/config.h
    Simulador_avion/model_config.h
)

# ============================================================
# CREAR EJECUTABLE
# ============================================================

add_executable(Simulador_avion ${SOURCES} ${HEADERS})

# ============================================================
# INCLUIR DIRECTORIOS
# ============================================================

target_include_directories(Simulador_avion PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Simulador_avion/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Simulador_avion
)

# ============================================================
# VINCULAR LIBRERÍAS
# ============================================================

target_link_libraries(Simulador_avion
    glfw
    GLEW::GLEW
    OpenGL::OpenGL
    assimp::assimp
    glm::glm
)

# ============================================================
# POST-BUILD: COPIAR SHADERS
# ============================================================

add_custom_command(TARGET Simulador_avion POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/Simulador_avion/shaders
    $<TARGET_FILE_DIR:Simulador_avion>/shaders
    COMMENT "Copiando shaders GLSL..."
)

# ============================================================
# OPCIONES DE COMPILACIÓN
# ============================================================

if(MSVC)
    target_compile_options(Simulador_avion PRIVATE /W3)
else()
    target_compile_options(Simulador_avion PRIVATE -Wall -Wextra)
endif()

# ============================================================
# MENSAJE DE ÉXITO
# ============================================================

message(STATUS "✓ Proyecto configurado exitosamente")
message(STATUS "  Ejecutable: ${CMAKE_BINARY_DIR}/Simulador_avion")
message(STATUS "  Shaders serán copiados a: $<TARGET_FILE_DIR:Simulador_avion>/shaders")
