#version 330 core

in vec2 TexCoord;
in vec3 FragPos;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gReflect_mask;

uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform float moveOffset;

const float distStrength = 0.004;


void main()
{
    gPosition = FragPos;

    vec2 distortedTexCoords = texture(dudvMap, vec2(TexCoord.x+moveOffset, TexCoord.y)).rg*0.1;
	distortedTexCoords = TexCoord + vec2(distortedTexCoords.x, distortedTexCoords.y+moveOffset);

    vec4 normalMapColor = texture(normalMap, distortedTexCoords);
    //Get normals in world space
    vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 5.0, normalMapColor.g * 2.0 - 1.0);
    gNormal = normalize(normal);
    //gAlbedoSpec = vec4(0.0f, 0.0f, 0.0f, 0.2f);
    gReflect_mask = vec3(0.5f, 0.0f, 0.0f);
}