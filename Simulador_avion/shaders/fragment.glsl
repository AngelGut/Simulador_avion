#version 330 core

in VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec3 color;
    vec2 texCoords;
} fs_in;

out vec4 FragColor;

uniform vec3 uLightPos;
uniform vec3 uViewPos;
uniform vec3 uLightColor;

uniform sampler2D uTexture;
uniform bool uUseTexture;

void main() {
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
