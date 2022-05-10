#version 330 core

in vec3 TexCoord;

uniform samplerCube skybox;
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gReflect_mask;

in vec3 FragPos;
void main() {
	gPosition = FragPos;
	gAlbedoSpec = texture(skybox, TexCoord)+vec4(0.0,0.3,0.4,0);
	gAlbedoSpec.w = 0;
	gNormal = vec3(0.0,0.0,0.0);
	gReflect_mask = vec3(0.0,1.0,0.0);
}