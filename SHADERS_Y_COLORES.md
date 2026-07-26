# 🎨 Guía de Shaders y Colores - Fase B

## 📚 Conceptos Básicos

### ¿Qué son los Shaders?

Los shaders son pequeños programas que corren en la GPU para procesar vértices y píxeles:

- **Vertex Shader**: Procesa cada vértice (posición, normal, etc.)
- **Fragment Shader**: Calcula el color final de cada píxel

### Pipeline de Renderizado (Simplificado)

```
Modelo OBJ (Assimp)
    ↓
Vértices + Normales + Colores (CPU → GPU via VAO/VBO)
    ↓
Vertex Shader (GPU) → Transforma coordenadas + pasa normales y colores
    ↓
Rasterización (GPU) → Crea fragmentos entre vértices
    ↓
Fragment Shader (GPU) → Calcula iluminación Phong + aplica color
    ↓
Framebuffer → Pantalla
```

## 🔵 Vertex Shader (`shaders/vertex.glsl`)

```glsl
#version 330 core
layout(location = 0) in vec3 aPosition;    // Posición del vértice
layout(location = 1) in vec3 aNormal;      // Normal del vértice
layout(location = 2) in vec3 aColor;       // Color del vértice o material

uniform mat4 uModel;      // Matriz de transformación del modelo
uniform mat4 uView;       // Matriz de vista (cámara)
uniform mat4 uProjection; // Matriz de proyección (perspectiva)

out VS_OUT {
    vec3 fragPos;   // Posición en espacio mundial
    vec3 normal;    // Normal transformada
    vec3 color;     // Color a interpolar
} vs_out;

void main() {
    // Transformar posición al espacio mundial
    vs_out.fragPos = vec3(uModel * vec4(aPosition, 1.0));
    
    // Transformar normal (importante: usar inverse transpose)
    vs_out.normal = normalize(mat3(transpose(inverse(uModel))) * aNormal);
    
    // Pasar color tal como está
    vs_out.color = aColor;

    // Posición final en espacio de clip
    gl_Position = uProjection * uView * vec4(vs_out.fragPos, 1.0);
}
```

### Explicación de Conceptos Clave:

**`layout(location = N)`**: Vincula atributos a índices de VAO:
- location 0 = posición
- location 1 = normal
- location 2 = color

**`uniform`**: Variables globales que se pasan desde CPU
- Se mantienen iguales para todos los vértices en un draw call
- Se actualizan en `main.cpp` con `shader.setMat4()`, etc.

**Transformación de Normales**:
```
normal = mat3(transpose(inverse(uModel))) * aNormal
```
Esta fórmula es crucial para que la iluminación funcione correctamente cuando hay escalado no uniforme.

## 🔴 Fragment Shader (`shaders/fragment.glsl`)

```glsl
#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec3 color;
} fs_in;

out vec4 FragColor;  // Color final del píxel

// Uniforms de iluminación
uniform vec3 uLightPos;   // Posición de la luz
uniform vec3 uViewPos;    // Posición de la cámara
uniform vec3 uLightColor; // Color de la luz

void main() {
    // 1. ILUMINACIÓN AMBIENTAL
    // La luz ambiental da valor mínimo a objetos oscuros
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * uLightColor;

    // 2. ILUMINACIÓN DIFUSA
    // Depende del ángulo entre normal y dirección de luz
    vec3 norm = normalize(fs_in.normal);
    vec3 lightDir = normalize(uLightPos - fs_in.fragPos);
    float diff = max(dot(norm, lightDir), 0.0);  // Producto punto
    vec3 diffuse = diff * uLightColor;

    // 3. ILUMINACIÓN ESPECULAR (Highlights brillantes)
    // Simula la reflexión en superficies lisas
    float specularStrength = 0.5;
    vec3 viewDir = normalize(uViewPos - fs_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * uLightColor;

    // 4. COMBINAR COMPONENTES CON COLOR DEL VÉRTICE
    vec3 result = (ambient + diffuse + specular) * fs_in.color;
    FragColor = vec4(result, 1.0);
}
```

### Fórmula de Iluminación Phong:

```
Color_Final = (Ambiental + Difusa + Especular) × Color_Vertice

Ambiental   = ambientStrength × luces
Difusa      = max(0, N·L) × luces
Especular   = (N·L)^32 × especularStrength × luces

Donde:
  N = normal normalizada
  L = dirección hacia la luz
  V = dirección hacia observador
  R = reflexión de L sobre N
```

## 🎯 Cómo se Importan los Colores

### Flujo en `model_loader.cpp`:

```cpp
// 1. Para cada mesh en el modelo
for (auto& mesh : meshes) {
    
    // 2. Obtener material asociado
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    
    // 3. Extraer color difuso
    glm::vec3 meshColor = extractColorFromMaterial(material);
    
    // 4. Para cada vértice
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        
        // Si el vértice tiene colores por vértice, usarlos
        if (mesh->HasVertexColors(0)) {
            vertex.color = glm::vec3(mesh->mColors[0][i].r, 
                                     mesh->mColors[0][i].g, 
                                     mesh->mColors[0][i].b);
        } else {
            // Si no, usar color del material
            vertex.color = meshColor;
        }
        
        vertices.push_back(vertex);
    }
}
```

### Prioridad de Colores:

1. **Color por Vértice** (si existe en OBJ/FBX) ← Máxima prioridad
2. **Color de Material** (diffuse color) ← Prioridad media
3. **Gris por defecto** (0.8, 0.8, 0.8) ← Fallback

### Extraer Colores desde Materiales:

```cpp
glm::vec3 Model::extractColorFromMaterial(aiMaterial* material) {
    aiColor4D color(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Intentar obtener color difuso (AI_MATKEY_COLOR_DIFFUSE)
    if (aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS) {
        return glm::vec3(color.r, color.g, color.b);
    }
    
    // Fallback: gris neutro
    return glm::vec3(0.8f, 0.8f, 0.8f);
}
```

## 🔗 Vinculación Shader ↔ VAO

### En C++ (`main.cpp`):

```cpp
// Compilar shader
Shader* shader = new Shader("shaders/vertex.glsl", "shaders/fragment.glsl");

// Usar shader
shader->use();

// Pasar matrices al shader
shader->setMat4("uModel", model);
shader->setMat4("uView", view);
shader->setMat4("uProjection", projection);

// Pasar luz al shader
shader->setVec3("uLightPos", glm::vec3(5.0f, 5.0f, 5.0f));
shader->setVec3("uViewPos", cameraPos);
shader->setVec3("uLightColor", glm::vec3(0.9f, 0.9f, 0.9f));

// Dibujar mesh
mesh.draw();  // Vincula VAO y dibuja
```

### En OpenGL (Automático):

```
Vertex Shader recibe:
  location 0 → aPosition (VAO buffer 0)
  location 1 → aNormal   (VAO buffer 1)
  location 2 → aColor    (VAO buffer 2)

Fragment Shader recibe:
  Valores interpolados de VS_OUT desde Vertex Shader
```

## 🛠️ Personalizar Iluminación

### Cambiar Tipo de Luz

En `fragment.glsl`, modifica estos parámetros:

```glsl
// Luz puntual (actual)
vec3 lightDir = normalize(uLightPos - fs_in.fragPos);

// Luz direccional (como el sol)
vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));

// Luz spot (como una linterna)
float theta = dot(lightDir, -spotDir);
if (theta > cos(cutoff)) { // Dentro del cono
    // Aplicar iluminación
}
```

### Cambiar Intensidad de Brillo

```glsl
// Menos brillo (superficie mate)
float spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);

// Más brillo (superficie pulida)
float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128.0);
```

### Cambiar Colores

```glsl
// Color con tono rojo
FragColor = vec4(result * vec3(1.0, 0.5, 0.5), 1.0);

// Escala de grises
float gray = dot(result, vec3(0.299, 0.587, 0.114));
FragColor = vec4(vec3(gray), 1.0);
```

## 📊 Ejemplo: Cargar un Modelo con Colores

### Archivo OBJ con Material (.mtl):

```obj
# boeing.obj
mtllib boeing.mtl
usemtl fuselage
v 0.0 0.0 0.0
v 1.0 0.0 0.0
v 0.5 1.0 0.0
f 1 2 3
```

```mtl
# boeing.mtl
newmtl fuselage
Ka 1.0 1.0 1.0
Kd 0.8 0.8 0.8    ← Este es el color que se importa
Ks 1.0 1.0 1.0
```

### Carga en C++:

```cpp
Renderer::loadModelByNumber(1);
// ↓
Model::loadModel("models/boeing.obj");
// ↓
Assimp lee boeing.mtl y extrae Kd (0.8, 0.8, 0.8)
// ↓
extractColorFromMaterial() retorna (0.8, 0.8, 0.8)
// ↓
Todos los vértices del fuselaje se colorean de (0.8, 0.8, 0.8)
// ↓
Fragment Shader aplica: (iluminación) × (0.8, 0.8, 0.8)
```

## 🎨 Próximos Pasos: Texturas

Para agregar texturas en futuras fases:

```glsl
uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

void main() {
    // Leer color desde textura
    vec3 texColor = texture(diffuseMap, fs_in.texCoord).rgb;
    
    // Combinar con iluminación
    vec3 result = (ambient + diffuse + specular) * texColor;
}
```

---

**¡Los shaders son el corazón de la Fase B moderna!**
