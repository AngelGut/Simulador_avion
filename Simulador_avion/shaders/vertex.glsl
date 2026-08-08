#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform int uSimMode;  // 0=NONE, 1=WIND, 2=THERMAL, 3=STRESS, 4=VIBRATION
uniform float uTime;

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec3 color;
    vec2 texCoords;
    vec3 localPos;     // Necesario para calculo de mapas termicos/esfuerzos
} vs_out;

void main() {
    vec3 position = aPosition;

    // Animacion modal (vibracion)
    if (uSimMode == 4) {
        // Las alas estan a lo largo del eje X, la cola en el eje Z (atras)
        float wingWeight = smoothstep(0.3, 3.5, abs(aPosition.x));
        float tailWeight = smoothstep(0.4, 2.5, abs(aPosition.z)) * 0.4;
        float totalWeight = wingWeight + tailWeight;
        
        // Oscilar en el eje Y vertical con una onda senoidal rapida
        position.y += sin(uTime * 18.0) * 0.08 * totalWeight;
    }

    vs_out.fragPos = vec3(uModel * vec4(position, 1.0));
    vs_out.normal = normalize(mat3(transpose(inverse(uModel))) * aNormal);
    vs_out.color = aColor;
    vs_out.texCoords = aTexCoords;
    vs_out.localPos = aPosition;

    gl_Position = uProjection * uView * vec4(vs_out.fragPos, 1.0);
}
