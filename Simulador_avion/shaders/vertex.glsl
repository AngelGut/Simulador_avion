#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aColor;
layout(location = 3) in vec2 aTexCoords;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out VS_OUT {
    vec3 fragPos;
    vec3 normal;
    vec3 color;
    vec2 texCoords;
} vs_out;

void main() {
    vs_out.fragPos = vec3(uModel * vec4(aPosition, 1.0));
    vs_out.normal = normalize(mat3(transpose(inverse(uModel))) * aNormal);
    vs_out.color = aColor;
    vs_out.texCoords = aTexCoords;

    gl_Position = uProjection * uView * vec4(vs_out.fragPos, 1.0);
}
