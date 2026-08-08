# ARQ002 — Iluminación, Shaders y Estética HUD

**Proyecto:** Visor 3D Interactivo de Aeronaves  
**Fecha:** Julio/Agosto 2026  

---

## 1. Iluminación por Shaders en Tiempo Real

### Decisión
Eliminar todas las referencias a iluminación por hardware de OpenGL 1.1 y calcular la reflectividad en el fragment shader (`fragment.glsl`) a partir de la posición de la luz cenital y la dirección de la cámara.

### Especificación del Modelo Phong
1. **Luz Ambiental:** Factor de iluminación base constante (`ambientStrength = 0.3`) para evitar caras completamente negras.
2. **Luz Difusa:** Calcula la incidencia perpendicular del rayo de luz sobre la normal de la cara (`max(dot(norm, lightDir), 0.0)`).
3. **Luz Especular:** Simula el brillo brillante de las aleaciones metálicas y la pintura de las aeronaves a partir del vector de reflexión y el punto de vista de la cámara.

---

## 2. Texturizado y Colores de Materiales

* **Priorización de Colores de Materiales:** El motor lee los colores definidos para cada malla en Blender (colores difusos, ambientales, etc.) usando Assimp, y los inyecta en el atributo de color de cada vértice.
* **Texturas Embebidas:** El cargador de modelos y la clase `ModelRenderer` soportan la carga y mapeo de texturas de imagen embebidas en los modelos GLB.

---

## 3. Interfaz Visual HUD (Estilo Militar/Ficción)

Para lograr un acabado de nivel premium e interactivo:
* **Gama de Colores:** Fondo oscuro espacial (`0.09f, 0.09f, 0.13f`), textos secundarios en gris medio, y realces dinámicos en amarillo neón y cian militar.
* **Barra Lateral de Diagnóstico:** Renderiza marcos de recuadros transparentes y barras de progreso divididas en 5 bloques discretos para mostrar el estado técnico del avión de forma estética.
* **Textos en Pantalla:** Implementación de contornos con sombras (`drop-shadow`) y sobre-dibujo de fuentes para crear un efecto de negrita (bold) y hacer el texto altamente legible sobre los modelos 3D en movimiento.