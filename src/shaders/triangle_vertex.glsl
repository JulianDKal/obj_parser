#version 450 core

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec3 vertexColor;
layout(location = 2) in vec2 textureCoordinate;

out vec4 colorVal;
out vec2 texCoord;

void main() {
    colorVal = vec4(vertexColor, 1);
    texCoord = textureCoordinate;
    gl_Position = vec4(vertexPos.x, -vertexPos.y, vertexPos.z, 1.0);
}