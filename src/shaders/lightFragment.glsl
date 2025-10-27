#version 450 core

in vec2 uv;
uniform sampler2D lightTexture;

out vec4 fragColor;

void main() {
    fragColor = texture(lightTexture, uv);
}