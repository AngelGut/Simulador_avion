# Guía de Integración para el Menú Principal 🚀

¡Hola! Esta guía está diseñada para que puedas integrar el **Menú Principal** del simulador de aviones de forma sencilla, utilizando las funciones y rutas que ya están establecidas en el código sin romper el sistema de renderizado ni la navegación del Modo Piezas.

---

## 📂 1. Ubicación de los Modelos y Rutas

Todos los modelos de aviones actuales están en formato **GLB** y sus rutas y nombres oficiales están centralizados en `config.h`. 

Las constantes que debes usar son las siguientes:

| ID del Avión | Constante de Nombre (`config.h`) | Constante de Ruta (`config.h`) | Archivo Físico |
| :---: | :--- | :--- | :--- |
| **1** | `NAME_1` ("A-10 Thunderbolt II") | `MODEL_1` | `modelos 3d/a-10_thunderbolt_ii.glb` |
| **2** | `NAME_2` ("B-24 Liberator") | `MODEL_2` | `modelos 3d/b-24_liberator.glb` |
| **3** | `NAME_3` ("Boeing 787 Dreamliner") | `MODEL_3` | `modelos 3d/boeing-787-_dreamliner.glb` |
| **4** | `NAME_4` ("MiG-29") | `MODEL_4` | `modelos 3d/mig_29_9-13.glb` |

---

## 🛠️ 2. Cómo cambiar de avión por código (API Interna)

Para cargar un avión nuevo desde tu menú principal (ya sea usando botones en pantalla, teclado o consola), debes invocar la siguiente función del espacio de nombres `Renderer`:

```cpp
#include "renderer.h"

// Cargar el avión deseado pasando su ID (1 al 4)
Renderer::loadModelByNumber(int modelNumber);
```

### Ejemplo de flujo completo al seleccionar un avión:
Cuando el usuario haga clic en un avión en tu menú, el código ideal para ejecutar es:

```cpp
// 1. Cargamos el modelo en memoria
Renderer::loadModelByNumber(idSeleccionado); // idSeleccionado debe ser 1, 2, 3 o 4

// 2. Ajustamos la cámara al zoom recomendado del nuevo avión (evita que se vea muy lejos o muy cerca)
Model* nuevoModel = Renderer::getLoadedModel();
if (nuevoModel) {
    viewZoom = nuevoModel->getRecommendedZoom(); // Ajusta el radio de la órbita de la cámara
}
```

*Nota: `viewZoom` es una variable global en `main.cpp` que controla la distancia de la cámara orbital. Si tu menú se implementa en un archivo separado, asegúrate de actualizar esta variable o exponer un helper.*

---

## 🗂️ 3. Acceso a metadatos dinámicos (Alternativa avanzada)

Si prefieres obtener la lista de aviones de forma dinámica en lugar de hardcodear los IDs, puedes incluir `model_config.h` y leer el arreglo global `AVAILABLE_MODELS`:

```cpp
#include "model_config.h"

// Iterar sobre los modelos configurados en el proyecto
for (int i = 0; i < ModelConfig::NUM_MODELS; i++) {
    std::string nombre = ModelConfig::AVAILABLE_MODELS[i].name;
    std::string ruta = ModelConfig::AVAILABLE_MODELS[i].path;
    std::string carpetaPiezas = ModelConfig::AVAILABLE_MODELS[i].partsFolder;
    
    // Aquí puedes renderizar tu botón del menú usando estos strings
}
```

---

## ⚠️ 4. Recomendaciones importantes para no romper nada

1. **Mantener IDs entre 1 y 4:** El rango actual de aviones configurado es estricto del `1` al `4`. Cargar un número fuera de este rango imprimirá un error en la consola y no cargará ningún modelo.
2. **Compatibilidad con Modo Piezas:** Al invocar `Renderer::loadModelByNumber()`, el sistema limpia automáticamente las texturas del avión anterior, desactiva el Modo Piezas (`partsModeActive = false`) y reinicia los contadores para que el nuevo avión empiece en vista completa y no haya mezclas de piezas de distintos modelos.
3. **Escenografía (Hangar):** La escenografía del hangar y las luces cenitales ya están automatizadas. Al cambiar de modelo con la función indicada, el piso del hangar se adaptará automáticamente a la nueva escala del avión.
