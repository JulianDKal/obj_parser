#version 450 core

in vec4 colorVal;
in vec2 texCoord;
out vec4 fragmentColor;

uniform float uniColorVal = 0;
uniform sampler2D brickTexture;

void main() {
    fragmentColor = 0.7 * texture(brickTexture, texCoord) + 0.3 * vec4(0, 0, uniColorVal, 1);
}