### 1. El fallo fatal de Assimp al cargar el Boeing (`GLTF2: data is null`)
* **El error:** Al intentar importar el Boeing 787 y sus interiores, la librería Assimp crasheaba la aplicación. Ocurría porque el exportador de Blender añadía bloques de animación vacíos o "morph targets" sin datos que Assimp no sabía interpretar.
* **La solución:** Creamos un script en Python (`fix_glb.py`) que preprocesaba el archivo `.glb` binario, limpiaba las animaciones rotas y reestructuraba los descriptores de datos antes de que C++ intentara leerlos.

### 2. El crasheo del driver de video de AMD (`atio6axx.dll` Access Violation)
* **El error:** Al implementar la primera precarga masiva de los 4 aviones y sus ~60 piezas, intentamos subir toda la geometría a la tarjeta gráfica en un único frame. Esto desbordó la cola de comandos del driver de AMD Radeon, congelando y crasheando el sistema.
* **La solución:** Rediseñamos el cargador como una máquina de estados secuencial (cargando exactamente un archivo por frame) y agregamos llamadas a `glFlush()`. Así, la GPU tiene tiempo para procesar cada lote y vaciar la cola de comandos entre fotogramas.

### 3. El tren de aterrizaje "acostado" del B-24 Liberator
* **El error:** Al cargar el tren de aterrizaje del B-24 (`b24_tren.glb`), este aparecía rotado 90 grados respecto al cuerpo del avión, flotando de forma antinatural fuera de su sitio.
* **La solución:** En lugar de forzar a editar el modelo original en Blender, creamos un parche directo en memoria en C++: detectamos si el archivo cargado contiene la palabra "b24_tren" y le aplicamos una rotación matemática matricial de 90° en el eje X a todos sus vértices al momento de cargarlo.

### 4. El túnel de viento "al revés" (El aire pegaba por la cola)
* **El error:** La simulación de las líneas de flujo de aire en el túnel de viento golpeaba al A-10, B-24 y MiG-29 por la parte trasera (la cola) y avanzaba hacia la cabina, lo cual era físicamente incorrecto (el Boeing era el único que estaba bien).
* **La solución:** Modificamos la ecuación del shader de viento para que, al detectar cualquier avión que no fuera el Boeing, invirtiera el signo del tiempo en la animación de las coordenadas, haciendo que el flujo de viento viaje correctamente de la nariz hacia la cola.

### 5. El botón "Volver" que te expulsaba del visor
* **El error:** En el visor 3D, cuando el usuario entraba al "Modo Piezas" (para ver partes flotantes del avión) y presionaba el botón "Volver", el simulador lo sacaba de golpe al menú principal en lugar de regresarlo a ver el avión completo.
* **La solución:** Reestructuramos el flujo de la interfaz del visor para crear una navegación por niveles: el primer clic en "Volver" desactiva el desarmado de piezas (regresando al avión entero) y solo un segundo clic te regresa al menú de selección.

### 6. El error de enlazado de Audio (`LNK2019` Unresolved Externals)
* **El error:** Al integrar la música del hangar y los sonidos de motores individuales, el compilador de Visual Studio fallaba al compilar el ejecutable porque no encontraba las funciones de audio debido a archivos fuente no agregados o mal vinculados.
* **La solución:** Corregimos la estructura del proyecto en el archivo de configuración de MSBuild para forzar la compilación del módulo `audio_manager.cpp` e incluir correctamente las librerías de bajo nivel.

### 7. Texto del HUD cortado y de difícil lectura
* **El error:** Las descripciones detalladas de los hotspots de los aviones aparecían en una sola línea continua que se salía de los límites del HUD flotante y de la pantalla. Además, la fuente blanca se perdía visualmente contra los aviones de colores similares.
* **La solución:** Programamos un algoritmo manual de envoltura de texto (*Word-wrap*) limitado a 36 caracteres por línea y dibujamos un doble texto desfasado (sombra negra de fondo) para garantizar alto contraste y legibilidad sobre cualquier superficie.

### 8. Pérdida de texturas en el Hangar
* **El error:** Al entrar y salir del visor de aviones, a veces el piso del hangar o las líneas del túnel de viento perdían sus texturas o se volvían de color negro sólido.
* **La solución:** Ocurría porque los shaders compartían variables uniformes globales y algunos estados de iluminación quedaban activos en la GPU. Añadimos un guardado y restauración estricta de uniformes (`uUseTexture` y `uUseLighting`) antes y después de dibujar cada elemento de la escena.
