#version 330 core
out vec4 FragColor;

uniform vec3 viewPos;
in vec3 FragPos;
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gReflect_mask;

void main()
{
    gPosition = FragPos;
	gAlbedoSpec = vec4(1.0);
	gAlbedoSpec.w = 0;
	gNormal = vec3(0.0,0.0,0.0);
	gReflect_mask = vec3(0.0,1.0,0.0);
}