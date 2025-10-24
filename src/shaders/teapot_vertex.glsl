#version 450 core
layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 textureCoordinate;

uniform float scale = 1;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vNormal;
out vec3 FragPos;
out vec2 texCoord;

void main() {
    vNormal = aNormal;
    texCoord = textureCoordinate;
    FragPos = vec3(model * vec4(vertexPos, 1.0));
    vec3 normalizedPos = vec3(vertexPos.x / scale, vertexPos.y / scale, vertexPos.z / scale);
    gl_Position = projection * view * model * vec4(normalizedPos, 1.0);
}