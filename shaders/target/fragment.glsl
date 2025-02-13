#version 410 core

out vec4 outColor;

in vec2 UV;
uniform sampler2D costumeTexture;

void main() {
    outColor = texture(costumeTexture, UV);
}