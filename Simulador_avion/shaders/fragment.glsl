#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec3 color;
    vec2 texCoords;
    vec3 localPos;
} fs_in;

out vec4 FragColor;

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;

uniform sampler2D uTexture;
uniform bool uUseTexture;
uniform bool uUseLighting;

uniform int uSimMode;   // 0=NONE, 1=WIND, 2=THERMAL, 3=STRESS, 4=VIBRATION
uniform int uMeshType;  // 0=Default, 1=Engine, 2=Wing
uniform float uTime;

// Rampas de color para visualizaciones de ingenieria
vec3 getThermalColor(float t) {
    // Azul vibrante (frio) -> Verde -> Amarillo -> Rojo (caliente)
    if (t < 0.25) {
        return mix(vec3(0.05, 0.25, 0.8), vec3(0.0, 0.8, 0.8), t / 0.25);
    } else if (t < 0.5) {
        return mix(vec3(0.0, 0.8, 0.8), vec3(0.0, 0.8, 0.0), (t - 0.25) / 0.25);
    } else if (t < 0.75) {
        return mix(vec3(0.0, 0.8, 0.0), vec3(0.9, 0.9, 0.0), (t - 0.5) / 0.25);
    } else {
        return mix(vec3(0.9, 0.9, 0.0), vec3(1.0, 0.0, 0.0), (t - 0.75) / 0.25);
    }
}

vec3 getStressColor(float s) {
    // Azul vibrante (bajo) -> Verde -> Amarillo -> Rojo -> Magenta (esfuerzo critico)
    if (s < 0.3) {
        return mix(vec3(0.05, 0.25, 0.8), vec3(0.0, 0.8, 0.4), s / 0.3);
    } else if (s < 0.6) {
        return mix(vec3(0.0, 0.8, 0.4), vec3(0.9, 0.8, 0.0), (s - 0.3) / 0.3);
    } else if (s < 0.85) {
        return mix(vec3(0.9, 0.8, 0.0), vec3(1.0, 0.1, 0.0), (s - 0.6) / 0.25);
    } else {
        return mix(vec3(1.0, 0.1, 0.0), vec3(0.8, 0.0, 0.8), (s - 0.85) / 0.15);
    }
}

void main() {
    // 1. Verificar modos de simulacion de ingenieria
    if (uSimMode == 1) {
        // Lineas de flujo del tunel de viento animadas
        float pulse = sin(fs_in.texCoords.x * 35.0 - uTime * 12.0) * 0.5 + 0.5;
        pulse = pow(pulse, 8.0); // Concentrar el brillo en un pulso definido
        vec3 col = mix(fs_in.color, vec3(1.0, 1.0, 1.0), pulse * 0.7);
        FragColor = vec4(col, 1.0);
        return;
    }

    // Calcular sombreado difuso sutil para dar volumen 3D en las simulaciones
    vec3 norm = normalize(fs_in.normal);
    vec3 lightDir = normalize(uLightPos - fs_in.fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    float shadingFactor = 0.4 + 0.6 * diff;

    if (uSimMode == 2) {
        // MODO TERMICO
        float T = 0.05 + 0.1 * smoothstep(1.5, -1.5, fs_in.localPos.z);
        if (uMeshType == 1) {
            // Motores calientes (combustion)
            T = 0.75 + 0.2 * (sin(uTime * 3.0) * 0.05 + 0.95);
        } else if (uMeshType == 2) {
            // Friccion en los bordes de ataque de las alas (Z negativo en coordenadas locales del ala)
            float leading = smoothstep(0.4, -0.4, fs_in.localPos.z);
            T = 0.1 + 0.5 * leading;
        }
        FragColor = vec4(getThermalColor(T) * shadingFactor, 1.0);
        return;
    }
    
    if (uSimMode == 3) {
        // MODO ESFUERZO MECANICO (VON MISES)
        float S = 0.08;
        if (uMeshType == 2) {
            // Tension en la raiz del ala (cerca de X=0) y pilones de carga
            float rootStress = smoothstep(2.5, 0.2, abs(fs_in.localPos.x)) * 0.7;
            S = 0.1 + rootStress + sin(uTime * 5.0) * 0.03 * rootStress;
        } else if (uMeshType == 1) {
            // Carga de peso en los pilones de motores
            S = 0.65 + sin(uTime * 8.0) * 0.02;
        }
        FragColor = vec4(getStressColor(S) * shadingFactor, 1.0);
        return;
    }

    // Si no se usa iluminacion, pintar color base directo (util para fondo o UI)
    if (!uUseLighting) {
        vec3 base = fs_in.color;
        if (uUseTexture) {
            vec4 texColor = texture(uTexture, fs_in.texCoords);
            if (texColor.a < 0.1) discard;
            base = texColor.rgb;
        }
        FragColor = vec4(base, 1.0);
        return;
    }

    // Iluminación ambiental
    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * uLightColor;

    // Iluminación difusa
    vec3 norm = normalize(fs_in.normal);
    vec3 lightDir = normalize(uLightPos - fs_in.fragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    // Iluminación especular (Phong)
    float specularStrength = 0.5;
    vec3 viewDir = normalize(uViewPos - fs_in.fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * uLightColor;

    // Color base: usar textura si está activa, de lo contrario color del vértice
    vec3 baseColor = fs_in.color;
    if (uUseTexture) {
        vec4 texColor = texture(uTexture, fs_in.texCoords);
        if (texColor.a < 0.1) discard; // Transparencia básica
        baseColor = texColor.rgb;
    }

    // Combinar iluminación con color base
    vec3 result = (ambient + diffuse + specular) * baseColor;
    FragColor = vec4(result, 1.0);
}
