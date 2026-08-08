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
uniform int uPlaneType; // 0=A10, 1=B24, 2=Boeing, 3=MiG

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
        float timeDirection = -12.0;
        if (uPlaneType != 2) {
            // Invertir sentido del flujo de aire para A-10 (0), B-24 (1) y MiG-29 (3)
            timeDirection = 12.0;
        }
        float pulse = sin(fs_in.texCoords.x * 35.0 + uTime * timeDirection) * 0.5 + 0.5;
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
        // MODO TERMICO (Foco en motores y fricción frontal)
        // Temperatura base (fricción en la nariz/punta frontal del avión)
        float T = 0.05;
        if (uPlaneType == 0) {
            // A-10: Nariz en Z positivo
            T += 0.15 * smoothstep(-0.3, 0.6, fs_in.localPos.z);
        } else {
            // Otros aviones (B-24, Boeing, MiG-29): Nariz en Z negativo
            T += 0.15 * smoothstep(0.3, -0.6, fs_in.localPos.z);
        }

        // Identificar volumen del cilindro de motores para calentamiento extremo (600°C+)
        bool isEngine = false;
        if (uMeshType == 1) {
            isEngine = true; // Clasificación por CPU
        } else {
            if (uPlaneType == 0) {
                // A-10: Motores gemelos traseros (centrados en X = ±0.12, Z de -0.58 a -0.05 para cubrir las tomas de aire delanteras)
                float distAxis = min(length(fs_in.localPos.xy - vec2(0.12, 0.08)), length(fs_in.localPos.xy - vec2(-0.12, 0.08)));
                if (fs_in.localPos.z > -0.58 && fs_in.localPos.z < -0.05 && distAxis < 0.09) {
                    isEngine = true;
                }
            } else if (uPlaneType == 1) {
                // B-24: 4 motores. Usamos las coordenadas simétricas exactas del modelo original:
                // Izquierdos y derechos: ±0.153 (int), ±0.353 (ext) con un radio robusto de 0.065 para cobertura completa
                float dInner = min(length(fs_in.localPos.xy - vec2(-0.153, 0.0)), length(fs_in.localPos.xy - vec2(0.153, 0.0)));
                float dOuter = min(length(fs_in.localPos.xy - vec2(-0.353, 0.0)), length(fs_in.localPos.xy - vec2(0.353, 0.0)));
                float distAxis = min(dInner, dOuter);
                if (fs_in.localPos.z > 0.09 && fs_in.localPos.z < 0.38 && distAxis < 0.065) {
                    isEngine = true;
                }
            } else if (uPlaneType == 2) {
                // Boeing 787: 2 grandes motores bajo las alas
                float distAxis = min(length(fs_in.localPos.xy - vec2(0.24, -0.07)), length(fs_in.localPos.xy - vec2(-0.24, -0.07)));
                if (fs_in.localPos.z > -0.18 && fs_in.localPos.z < 0.02 && distAxis < 0.065) {
                    isEngine = true;
                }
            } else if (uPlaneType == 3) {
                // MiG-29: toberas de escape traseras
                float distAxis = min(length(fs_in.localPos.xy - vec2(0.06, 0.0)), length(fs_in.localPos.xy - vec2(-0.06, 0.0)));
                if (fs_in.localPos.z > 0.28 && fs_in.localPos.z < 0.44 && distAxis < 0.055) {
                    isEngine = true;
                }
            }
        }

        if (isEngine) {
            // Motores a máxima temperatura
            T = 0.85 + 0.15 * (sin(uTime * 4.0) * 0.05 + 0.95);
        } else {
            // Aplicar calor difuso sutil en la zona exterior del motor (difuminado suave)
            float distEng = 9.9;
            if (uPlaneType == 0) {
                distEng = min(length(fs_in.localPos - vec3(0.12, 0.08, -0.42)), length(fs_in.localPos - vec3(-0.12, 0.08, -0.42)));
            } else if (uPlaneType == 1) {
                float d1 = length(fs_in.localPos - vec3(-0.353, 0.0, 0.20));
                float d2 = length(fs_in.localPos - vec3(-0.153, 0.0, 0.20));
                float d3 = length(fs_in.localPos - vec3(0.153, 0.0, 0.20));
                float d4 = length(fs_in.localPos - vec3(0.353, 0.0, 0.20));
                distEng = min(min(d1, d2), min(d3, d4));
            } else if (uPlaneType == 2) {
                distEng = min(length(fs_in.localPos - vec3(0.24, -0.07, -0.08)), length(fs_in.localPos - vec3(-0.24, -0.07, -0.08)));
            } else if (uPlaneType == 3) {
                distEng = min(length(fs_in.localPos - vec3(0.06, 0.0, 0.36)), length(fs_in.localPos - vec3(-0.06, 0.0, 0.36)));
            }
            // Aumentamos el denominador a 0.035 y el factor a 0.65 para un difuminado más suave y expansivo
            float heat = exp(-distEng * distEng / 0.035);
            T = max(T, 0.05 + 0.65 * heat);
        }

        // Fricción en los bordes de ataque de las alas (abs(x) > 0.15, Z frontal)
        if (abs(fs_in.localPos.x) > 0.15) {
            float wingFriction = smoothstep(0.15, 0.70, abs(fs_in.localPos.x)) * smoothstep(0.10, -0.12, fs_in.localPos.z) * 0.48;
            T = max(T, wingFriction);
        }

        FragColor = vec4(getThermalColor(T) * shadingFactor, 1.0);
        return;
    }
    
    if (uSimMode == 3) {
        // MODO ESFUERZO MECANICO (VON MISES)
        float S = 0.08;

        // Esfuerzo de tensión en la raíz del ala (unión con el fuselaje, abs(X) entre 0.10 y 0.28, Z cerca de 0)
        float rootStress = smoothstep(0.35, 0.12, abs(fs_in.localPos.x)) * 
                           smoothstep(0.08, 0.22, abs(fs_in.localPos.x)) * 
                           exp(-fs_in.localPos.z * fs_in.localPos.z / 0.03) * 0.70;

        // Esfuerzo en los soportes/pilones de motores
        float distMount = 9.9;
        if (uMeshType == 1) {
            distMount = 0.0;
        } else {
            if (uPlaneType == 0) {
                // A-10: Soporte del pod de motores trasero
                distMount = min(length(fs_in.localPos - vec3(0.12, 0.08, -0.42)), length(fs_in.localPos - vec3(-0.12, 0.08, -0.42)));
            } else if (uPlaneType == 1) {
                // B-24: 4 pilones de motores de ala
                float d1 = length(fs_in.localPos - vec3(-0.353, 0.0, 0.20));
                float d2 = length(fs_in.localPos - vec3(-0.153, 0.0, 0.20));
                float d3 = length(fs_in.localPos - vec3(0.153, 0.0, 0.20));
                float d4 = length(fs_in.localPos - vec3(0.353, 0.0, 0.20));
                distMount = min(min(d1, d2), min(d3, d4));
            } else if (uPlaneType == 2) {
                distMount = min(length(fs_in.localPos - vec3(0.25, -0.02, -0.05)), length(fs_in.localPos - vec3(-0.25, -0.02, -0.05)));
            } else if (uPlaneType == 3) {
                distMount = min(length(fs_in.localPos - vec3(0.06, 0.0, 0.25)), length(fs_in.localPos - vec3(-0.06, 0.0, 0.25)));
            }
        }

        float mountStress = exp(-distMount * distMount / 0.015) * 0.65;
        S = max(S, rootStress + mountStress);

        // Añadir una pequeña oscilación senoidal para simular fatiga de materiales y vibración por sustentación
        S = S * (sin(uTime * 5.0) * 0.03 + 0.97);

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
