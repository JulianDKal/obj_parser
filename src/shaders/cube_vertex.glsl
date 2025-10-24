#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 textureCoordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 colorVal;
out vec2 texCoord;

void main()
{
    colorVal = vec4(0, 0.2, 1, 1);
    texCoord = textureCoordinate;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
