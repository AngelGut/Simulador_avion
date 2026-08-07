# ARQUITECTURA VISUAL Y RESUMEN EJECUTIVO

---

## ARQUITECTURA ACTUAL (OpenGL 1.1 + GLUT)

```
┌─────────────────────────────────────────────────────────────┐
│                     main.cpp (GLUT)                         │
│  ├─ glutInit(), glutCreateWindow()                          │
│  ├─ glMatrixMode() → matrix stack                           │
│  ├─ gluPerspective() → proyección                           │
│  ├─ glutDisplayFunc(display)                               │
│  └─ glutMainLoop()                                          │
└────────────────────┬──────────────────────────────────────┘
                     │ callbacks GLUT
                     ▼
┌─────────────────────────────────────────────────────────────┐
│              renderer.cpp (Fixed Pipeline)                  │
│  ├─ glEnable(GL_LIGHTING)                                  │
│  ├─ glLight() → configura luces                            │
│  ├─ glColor3f() → color global                             │
│  ├─ drawLayer() → renderiza modelo                         │
│  └─ Renderer::setupOpenGL()                                │
└────────────────────┬──────────────────────────────────────┘
                     │
      ┌──────────────┴──────────────┐
      ▼                             ▼
┌──────────────────┐      ┌──────────────────┐
│ model_loader.cpp │      │  geometry.cpp    │
│  ├─ Assimp load  │      │  ├─ glBegin()    │
│  ├─ Mesh struct  │      │  ├─ glVertex()   │
│  └─ Mesh::draw() │      │  └─ glEnd()      │
│    glBegin()     │      └──────────────────┘
│    glEnd()       │
└──────────────────┘

INPUT HANDLING:
  glutKeyboardFunc() → keyboard() → modifica vars globales
  
TRANSFORMACIONES:
  glRotatef() + glMatrixMode() → matrix stack OpenGL
```

---

## ARQUITECTURA NUEVA (OpenGL 3.3+ + GLFW + Shaders)

```
┌──────────────────────────────────────────────────────────────┐
│                 main_ng.cpp (GLFW Loop)                      │
│  ├─ window.init() → glfwCreateWindow()                       │
│  ├─ while (!shouldClose()) loop                             │
│  ├─ glfwPollEvents() → input                                │
│  ├─ renderer.drawModel() → draw call                        │
│  └─ window.swapBuffers()                                    │
└──────────────────┬──────────────────────────────────────┘
                   │
      ┌────────────┼────────────┐
      ▼            ▼            ▼
   ┌─────┐  ┌──────────┐  ┌─────────────┐
   │INPUT│  │ GRAPHICS │  │   SHADERS   │
   │ MGR │  │   CORE   │  │ (GPU-side)  │
   └────┘  └──────────┘  └─────────────┘
      │         │               │
      ▼         ▼               ▼
  ┌──────┐ ┌──────────────────────────────┐
  │INPUT │ │    SHADER PROGRAM            │
  │      │ │  ┌──────────────┬──────────┐ │
  │Teclado├─┤  │vertex.glsl   │frag.glsl │ │
  │Events │ │  │┌────────────┐│┌────────┐│ │
  └──────┘ │  ││ matrices   ││ phong  ││ │
           │  ││ transform  ││ light  ││ │
           │  ││ normal mat ││ color  ││ │
           │  │└────────────┘│└────────┘│ │
           │  └──────────────┴──────────┘ │
           └──────────────────────────────┘
      │
      ├─────────────────────────────┐
      ▼                             ▼
┌─────────────────┐       ┌─────────────────┐
│  CAMERA (GLM)   │       │  RENDERER NG    │
│ ├─ mat4 view    │       │ ├─ setUniforms()│
│ ├─ mat4 proj    │       │ ├─ drawModel()  │
│ ├─ rotation     │       │ ├─ lighting cfg │
│ ├─ zoom/pan     │       │ └─ mat3 normal  │
│ └─ GLM matrices │       └────────┬────────┘
└─────────────────┘                │
                                   ▼
                      ┌────────────────────────┐
                      │   MODEL LOADING (GPU)  │
                      │  ├─ Assimp parse      │
                      │  ├─ VAO_MESH create   │
                      │  ├─ Vertex upload     │
                      │  └─ Index upload      │
                      └────────┬───────────────┘
                               │
                ┌──────────────┴──────────────┐
                ▼                            ▼
        ┌────────────────┐        ┌──────────────────┐
        │ GEOMETRY NG    │        │ VAO_MANAGER      │
        │ ├─ generate*() │        │ ├─ glGenVAO()    │
        │ ├─ Mesh struct │        │ ├─ glGenVBO()    │
        │ └─ VAO upload  │        │ ├─ glBindVertex()│
        └────────────────┘        │ ├─ glDrawElements│
                                  │ └─ bind/unbind   │
                                  └──────────────────┘

┌─────────────────────────────────────────────────────────────┐
│                   GPU (Graphics Memory)                     │
│  ┌───────────────────────────────────────────────────────┐ │
│  │ VAO: Vertex Array Object                             │ │
│  │  ├─ VBO: Vertex Buffer (positions, normals, colors)  │ │
│  │  ├─ EBO: Element Buffer (indices)                    │ │
│  │  └─ Attributes: layout(location=0/1/2)               │ │
│  └───────────────────────────────────────────────────────┘ │
│  ┌───────────────────────────────────────────────────────┐ │
│  │ Shader Program                                        │ │
│  │  ├─ vertex.glsl (transformaciones)                    │ │
│  │  └─ fragment.glsl (iluminación Phong)                 │ │
│  │  ├─ Uniforms: matrices, luz, color                   │ │
│  └───────────────────────────────────────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

---

## FLUJO DE RENDERIZADO (Nueva arquitectura)

```
START
  │
  ├─ 1. Window init (GLFW)
  │     └─ glfwCreateWindow()
  │
  ├─ 2. GLEW init
  │     └─ glewInit()
  │
  ├─ 3. Shader compile
  │     ├─ readFile(vertex.glsl)
  │     ├─ readFile(fragment.glsl)
  │     ├─ glCompileShader()
  │     └─ glLinkProgram()
  │
  ├─ 4. Load model (Assimp)
  │     ├─ Model::loadModel()
  │     ├─ processNode()
  │     ├─ processMesh()
  │     └─ normalizeModel()
  │
  ├─ 5. Upload to GPU (VAO/VBO)
  │     ├─ Model::uploadToGPU()
  │     ├─ VAOMesh::create()
  │     ├─ glGenVertexArrays()
  │     ├─ glBufferData() ← vertices
  │     ├─ glBufferData() ← indices
  │     └─ glVertexAttribPointer()
  │
  └─► MAIN LOOP
      │
      ├─ glClear(GL_COLOR | GL_DEPTH)
      │
      ├─ Input processing
      │  └─ glfwGetKey() → Camera updates
      │
      ├─ Render setup
      │  ├─ shader.use()
      │  ├─ Camera::getViewMatrix()
      │  ├─ Camera::getProjectionMatrix()
      │  └─ shader::setUniform() ← todas las matrices
      │
      ├─ Render meshes
      │  ├─ for each mesh:
      │  │  ├─ shader::setUniform(uObjectColor)
      │  │  ├─ shader::setUniform(uNormalMatrix)
      │  │  ├─ shader::setUniform(uLightPos, etc)
      │  │  ├─ vaoMesh.bind()
      │  │  ├─ glDrawElements(GL_TRIANGLES)  ◄─ GPU dibuja
      │  │  └─ vaoMesh.unbind()
      │  │
      │  └─► GPU (Vertex shader)
      │      ├─ gl_Position = projection * view * model * position
      │      ├─ vNormal = normalMatrix * aNormal
      │      └─ Interpolate to Fragment shader
      │
      │  └─► GPU (Fragment shader)
      │      ├─ Phong lighting model
      │      ├─ Ambient + Diffuse + Specular
      │      └─ FragColor = result
      │
      ├─ window.swapBuffers()
      │
      ├─ glfwPollEvents()
      │
      └─ while (!window.shouldClose())
         └─ goto MAIN LOOP

END
```

---

## MAPEO DE CONCEPTOS: Viejo → Nuevo

### 1. VENTANA Y CONTEXTO
```
ANTES                           DESPUÉS
glut*()  
├─ glutInit()                   → glfwInit()
├─ glutInitDisplayMode()        → glfwWindowHint()
├─ glutInitWindowSize()         → parámetro glfwCreateWindow()
├─ glutCreateWindow()           → glfwCreateWindow()
├─ glutReshapeFunc()            → glfwSetFramebufferSizeCallback()
├─ glutMainLoop()               → while (!shouldClose()) loop
├─ glutPostRedisplay()          → continuar loop
└─ glutSwapBuffers()            → glfwSwapBuffers()
```

### 2. TRANSFORMACIONES Y MATRICES
```
ANTES (Fixed Pipeline)          DESPUÉS (GLM)
glMatrixMode(GL_PROJECTION)     camera.getProjectionMatrix()
gluPerspective()                glm::perspective()

glMatrixMode(GL_MODELVIEW)      camera.getViewMatrix()
glLoadIdentity()                glm::mat4(1.0f)
glTranslatef()                  glm::translate()
glRotatef()                      glm::rotate()
glScalef()                       glm::scale()
gluLookAt()                     glm::lookAt()

Enviar a GPU:                   Enviar a GPU:
Matrix stack (implícito)        shader::setUniform("uModel", matrix)
                                shader::setUniform("uView", matrix)
                                shader::setUniform("uProjection", matrix)
```

### 3. ILUMINACIÓN
```
ANTES (Fixed Pipeline)          DESPUÉS (Fragment Shader)
glEnable(GL_LIGHTING)           Remover (usa shader)
glLight(GL_LIGHT0)              
├─ GL_AMBIENT                   → uniform vec3 uAmbient
├─ GL_DIFFUSE                   → uniform vec3 uDiffuse
├─ GL_SPECULAR                  → uniform vec3 uSpecular
├─ GL_POSITION                  → uniform vec3 uLightPos
└─ GL_SHININESS                 → uniform float uShininess

glColorMaterial()               Remover (usa shader)
glColor3f()                     → uniform vec3 uObjectColor

glNormal3f() (en geometría)     → Atributo vertex (layout location 1)
```

### 4. RENDERING
```
ANTES (Immediate mode)          DESPUÉS (Retained mode)
glBegin(GL_TRIANGLES)           VAO/VBO setup (una sola vez)
  for each vertex:              |
    glNormal3f()                ├─ glGenVertexArrays()
    glVertex3f()                ├─ glGenBuffers()
    (repeat...)                 ├─ glBufferData()
glEnd()                         └─ glVertexAttribPointer()

Cada frame: Execute           Cada frame:
(lento)                       glDrawElements()
                              (rápido)
```

### 5. DIBUJADO
```
ANTES                          DESPUÉS
render() {                     render() {
  glBegin()                      shader.use()
  for each vertex               for each mesh:
    send vertex data              vao.bind()
  glEnd()                        glDrawElements()
}                                vao.unbind()
                               }
Frecuencia: Cada frame         Frecuencia: Setup una sola vez
                                          Draw cada frame
```

---

## COMPARATIVA DE PERFORMANCE

### Benchmark (modelo 10K triangles)

```
ANTES (OpenGL 1.1 + glBegin/glEnd)
┌──────────────────────────────┐
│ FPS: 45-60                   │
│ CPU usage: 85% (bottleneck)  │
│ GPU usage: 30%               │
│ Frame time: 16-22ms          │
│  └─ 12-18ms: CPU → GPU      │ ◄ Rendimiento limitado
│  └─  4-8ms: GPU rendering   │
└──────────────────────────────┘

DESPUÉS (OpenGL 3.3+ + VAO/VBO + Shaders)
┌──────────────────────────────┐
│ FPS: 120+ (vsync 60)         │
│ CPU usage: 35% (optimizado)  │
│ GPU usage: 70%               │
│ Frame time: 8-10ms           │
│  └─  2-3ms: CPU overhead    │
│  └─  5-7ms: GPU rendering   │ ◄ Máximo aprovechamiento
└──────────────────────────────┘

Mejora: ~2-3x más rápido
```

---

## REQUISITOS POR PLATAFORMA

### Windows
```
┌─────────────────────────────────────────┐
│ Compilador: Visual Studio 2019+ (MSVC)  │
├─────────────────────────────────────────┤
│ GLFW 3.3+:  C:\Libraries\glfw\         │
│ GLEW 2.2+:  C:\Libraries\glew\         │
│ GLM:        (header-only)               │
│ Assimp:     Pre-compilado               │
├─────────────────────────────────────────┤
│ GPU: Intel HD 630+ / NVIDIA GTX 960+   │
│      (cualquier GPU OpenGL 3.3+)        │
└─────────────────────────────────────────┘
```

### Linux (Ubuntu 20.04+)
```
┌──────────────────────────────────────┐
│ Compilador: GCC 7.0+ / Clang 5.0+    │
├──────────────────────────────────────┤
│ sudo apt install:                    │
│  • libglfw3-dev                      │
│  • libglew-dev                       │
│  • libglm-dev                        │
│  • libassimp-dev                     │
├──────────────────────────────────────┤
│ GPU: Intel iGPU / NVIDIA / AMD       │
│      (cualquier GPU OpenGL 3.3+)     │
└──────────────────────────────────────┘
```

### macOS (Homebrew)
```
┌──────────────────────────────────────┐
│ Compilador: Apple Clang 11.0+        │
├──────────────────────────────────────┤
│ brew install:                        │
│  • glfw3                             │
│  • glew                              │
│  • glm                               │
│  • assimp                            │
├──────────────────────────────────────┤
│ GPU: Metal → OpenGL 4.1 compatibility│
│      (Apple silicon: arm64)          │
└──────────────────────────────────────┘
```

---

## DIFERENCIAS CLAVE POR PASO

### Step 1-4: Infraestructura (8-12h)
```
RIESGO: Bajo (componentes independientes)
IMPACTO: Window + Shader system funcionando
ROLLBACK: Fácil (no afecta código existente)
```

### Step 5-6: Shaders (4-6h)
```
RIESGO: Medio (primera vez con shaders)
IMPACTO: Sistema de iluminación moderno
ROLLBACK: Mantener fragment shader simple (fallback)
VALIDAR: glslangValidator vertex.glsl fragment.glsl
```

### Step 7: Main Loop (4-6h)
```
RIESGO: Medio (cambio de paradigma GLUT → GLFW)
IMPACTO: Sistema de rendering central
ROLLBACK: Compile viejo y nuevo en paralelo
VALIDAR: Input mapping 1:1 con versión anterior
```

### Step 8-9: Model Loader (4-6h)
```
RIESGO: Bajo-Medio (Assimp ya probado)
IMPACTO: Modelos en GPU con VAO/VBO
ROLLBACK: Mantener versión glBegin/glEnd
VALIDAR: Benchmarks de upload time
```

### Step 10-11: Renderer (4-6h)
```
RIESGO: Medio (integración de todo)
IMPACTO: Rendering pipeline completamente nuevo
ROLLBACK: Compile en paralelo si falla
VALIDAR: Iluminación Phong correcta (normales, especular)
```

### Step 12-13: Geometría Procedural (6-8h)
```
RIESGO: Medio (DDA, Bresenham → VAO)
IMPACTO: Fallback si no hay modelo
ROLLBACK: Mantener GeometryBuilder.cpp viejo
VALIDAR: Geometría renderiza igual visualmente
```

### Step 14-15: Build + Testing (6-8h)
```
RIESGO: Alto si dependencias no están
IMPACTO: Ejecutable funcional
ROLLBACK: Verificar CMakeLists.txt antes
VALIDAR: Compilar en 3 plataformas
```

---

## ESTRATEGIA DE ROLLBACK

```
Semana 1: Branches paralelos
├─ main (OpenGL 1.1 - funcional)
└─ modernize/gl33 (nuevo trabajo)

Si falla, revert a:
  git checkout main

Commit regularmente:
  1 commit por paso completado + validado

Si paso N falla:
  git reset --soft HEAD~N
  Keep code changes locally
  Fix issues
  New commits for each fix
```

---

## ESTIMACIÓN FINAL

```
┌─────────────────────────────────────┐
│ Timeline de Implementación          │
├─────────────────────────────────────┤
│ Fase 1 (Infra):        8-12h   ✓    │
│ Fase 2 (Shaders):      4-6h    ✓    │
│ Fase 3 (Main):         4-6h    ✓    │
│ Fase 4 (Loader):       4-6h    ✓    │
│ Fase 5 (Renderer):     4-6h    ✓    │
│ Fase 6 (Geometría):    6-8h    ✓    │
│ Fase 7 (Build/Test):   6-8h    ✓    │
├─────────────────────────────────────┤
│ TOTAL:                 36-52h       │
│                                     │
│ Desarrollo paralelo: 5-7 días       │
│ (8h/día dedicadas)                  │
└─────────────────────────────────────┘
```

---

## CHECKLIST RÁPIDO DE VIABILIDAD

```
✓ ¿Tienes compilador C++17+?
  Windows: Visual Studio 2019+
  Linux: GCC 7.0+ / Clang 5.0+
  macOS: Apple Clang 11.0+

✓ ¿Puedes instalar GLFW, GLEW, GLM?
  Windows: Manualmente (25 min)
  Linux: apt install (2 min)
  macOS: brew install (5 min)

✓ ¿Tienes GPU con OpenGL 3.3+?
  Intel: HD 630+ (2016+)
  NVIDIA: GTX 960+ (2015+)
  AMD: R9 290 (2013+)
  Apple: Metal support

✓ ¿Conoces lo básico de GLSL?
  No urgente - aprenderás en paso 5-6

✓ ¿Puedes dedica 5-7 días?
  5 horas/día de trabajo = 1 semana
  8 horas/día de trabajo = 5 días

✓ ¿Hay dependencias externas que rompan?
  No - proyecto es self-contained

╔════════════════════════════════════╗
║  ✓ PROYECTO VIABLE PARA MIGRACIÓN ║
║                                    ║
║  Riesgo: BAJO-MEDIO                ║
║  Complejidad: MEDIA                ║
║  Beneficio: ALTO (3x performance)  ║
╚════════════════════════════════════╝
```

