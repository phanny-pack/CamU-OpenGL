#version 330 core
in vec2 TexCoord;
uniform sampler2D uTex;
out vec4 FragColor;

void main() {
    FragColor = texture(uTex, TexCoord);
}