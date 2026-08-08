# ARQ003 — Ciclo de Vida y Gestión de Eventos (GLFW)

**Proyecto:** Visor 3D Interactivo de Aeronaves  
**Fecha:** Julio/Agosto 2026  

---

## 1. Ciclo de Vida del Contexto Gráfico (GLFW)

### Loop Principal
El simulador ha descartado por completo el ciclo de vida cerrado de `glutMainLoop()`. En su lugar, utiliza un bucle explícito controlado por la ventana de GLFW en `main.cpp`:

```cpp
while (!glfwWindowShouldClose(window)) {
    // 1. Calcular deltaTime
    float currentTime = (float)glfwGetTime();
    ctx.deltaTime = currentTime - ctx.lastFrameTime;
    ctx.lastFrameTime = currentTime;

    // 2. Procesar entrada de ratón contínua (pan/orbit)
    glfwGetCursorPos(window, &ctx.mouseX, &ctx.mouseY);

    // 3. Renderizar escena según el estado de la aplicación
    // (WELCOME, MENU, VIEWER, etc.)
    renderScene(window);

    // 4. Intercambiar buffers de pantalla (Double Buffering)
    glfwSwapBuffers(window);

    // 5. Encuestar eventos del sistema operativo (callbacks)
    glfwPollEvents();
}
```

---

## 2. Gestión de Eventos y Callbacks Modernos

### Eventos de Teclado (`keyCallback`)
Registrado mediante `glfwSetKeyCallback()`. Procesa acciones instantáneas de pulsación simple (`GLFW_PRESS`):
* **Tecla P / Retroceso:** Conmutación entre el visor completo y el despiece de piezas individuales.
* **Flechas Izquierda / Derecha:** Navegación secuencial entre las piezas desensambladas.
* **Tecla Espacio:** Restablece la rotación orbital y calcula el zoom ideal recomendado según las dimensiones físicas del avión seleccionado.
* **Tecla H:** Alterna la visualización del menú de ayuda rápida flotante.

### Eventos de Ratón (Movimiento Orbital e Interacción)
* **Arrastrar con Click Izquierdo:** Modifica los ángulos de rotación de la cámara (`viewRotationX`, `viewRotationY`) para orbitar alrededor del centro del avión.
* **Arrastrar con Click Derecho:** Modifica el desplazamiento en los ejes (`viewX`, `viewY`) para panear o trasladar el punto de enfoque de la cámara.
* **Callback de Rueda de Desplazamiento (`scrollCallback`):** Ajusta directamente el factor de zoom de la cámara (`viewZoom`), restringido para evitar planos de corte cercanos indeseados.
* **Callback de Click de Ratón (`mouse_button_callback`):** Gestiona la selección interactiva de los aviones en la pantalla del menú y los clics de botones del HUD.