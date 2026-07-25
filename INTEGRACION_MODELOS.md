# 🔌 Integración de Modelos 3D en main.cpp

## Opción 1: Integración Mínima (Recomendado)

Agregar estas líneas después de inicializar LayerManager:

```cpp
#include "model_manager_auto.h"

int main(int argc, char** argv) {
    // ... código existente de inicialización ...
    
    // Inicializar LayerManager (línea existente)
    layerManager.init();
    
    // ← AGREGAR ESTAS LÍNEAS:
    ModelManagerAuto& modelMgr = ModelManagerAuto::getInstance();
    int loadedCount = modelMgr.loadAllModelsFromFolder("./models/");
    if (loadedCount > 0) {
        modelMgr.printModelsInfo();
    }
    
    // ... resto del código ...
    glutMainLoop();
    return 0;
}
```

## Opción 2: Con Toggle de Modelos en UI

Si quieres agregar control interactivo (tecla M para mostrar/ocultar):

### Step 1: Agregar variable global
```cpp
#include "model_manager_auto.h"

ModelManagerAuto& modelMgr = ModelManagerAuto::getInstance();
bool showModels = true;  // ← NUEVO
```

### Step 2: Agregar control en keyboard()
```cpp
void keyboard(unsigned char key, int x, int y) {
    // ... código existente ...
    
    // ← AGREGAR ESTO:
    if (key == 'm' || key == 'M') {
        showModels = !showModels;
        std::cout << (showModels ? "✅ Modelos visibles\n" : "❌ Modelos ocultos\n");
        modelMgr.printModelsInfo();
        glutPostRedisplay();
        return;
    }
}
```

### Step 3: Renderizar modelos en display()
```cpp
void display() {
    // ... código existente ...
    
    glPushMatrix();
    {
        glTranslatef(viewX, viewY, 0.0f);
        glRotatef(viewRotation, 0.0f, 0.0f, 1.0f);
        glScalef(viewZoom, viewZoom, 1.0f);

        // Código existente
        Renderer::drawLayer(layerManager.getActiveLayer());
        
        // ← AGREGAR ESTO (después de drawLayer):
        if (showModels) {
            auto models = modelMgr.getAvailableModels();
            for (const auto& name : models) {
                Model* model = modelMgr.getModel(name);
                if (model && !model->isEmpty()) {
                    // Cuando implemente Renderer::drawModel():
                    // Renderer::drawModel(*model);
                }
            }
        }
    }
    glPopMatrix();
    
    // ... resto del código ...
}
```

## Opción 3: En main() con carga explícita

```cpp
int main(int argc, char** argv) {
    // ... inicialización ...
    
    ModelManagerAuto& modelMgr = ModelManagerAuto::getInstance();
    
    // Cargar modelos específicos
    modelMgr.loadAllModelsFromFolder("./models/");
    
    // Acceder a un modelo
    Model* falcon = modelMgr.getModel("Millennium Falcon star wars");
    if (falcon) {
        std::cout << "Falcon: " << falcon->vertices.size() << " vértices\n";
    }
    
    // ... resto ...
}
```

## Cambios Necesarios

| Qué | Dónde | Acción |
|-----|-------|--------|
| Include | main.cpp | `#include "model_manager_auto.h"` |
| Variable global | main.cpp | `ModelManagerAuto& modelMgr = ModelManagerAuto::getInstance();` |
| Init | main() | `modelMgr.loadAllModelsFromFolder("./models/");` |
| Keyboard (opcional) | keyboard() | Agregar tecla M |
| Renderizado (futuro) | display() | Llamar `Renderer::drawModel()` |

## Próximos Pasos

1. ✅ Subir archivos .obj/.fbx a `./models/`
2. ✅ Integrar ModelManagerAuto en main.cpp
3. ✅ Compilar y ejecutar
4. ⏳ Implementar `Renderer::drawModel()` para visualizar (requiere OpenGL)

## Nota

`main_with_models_example.cpp` fue eliminado para reducir duplicación.
Esta documentación contiene todos los snippets necesarios.

---

Ver también: `SETUP_MODELOS.md` para instrucciones de carga
