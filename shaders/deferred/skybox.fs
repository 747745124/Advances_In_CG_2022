#version 330 core

in vec3 TexCoord;

uniform samplerCube skybox;
out vec4 FragColor;

void main() {
	FragColor = texture(skybox, TexCoord)+vec4(0.0,0.3,0.4,0);
}