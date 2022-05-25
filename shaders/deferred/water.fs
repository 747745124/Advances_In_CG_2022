#version 330 core

in vec2 TexCoord;
in vec3 FragPos;

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gReflect_mask;
layout (location = 4) out vec2 gVelocity;

uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform float moveOffset;

const float distStrength = 0.004;

in mat4 trans_inv_view;
in vec4 lastUV;
in vec4 currUV;

void main()
{
    gPosition = FragPos;

    vec2 distortedTexCoords = texture(dudvMap, vec2(TexCoord.x+moveOffset, TexCoord.y)).rg*0.1;
	distortedTexCoords = TexCoord + vec2(distortedTexCoords.x, distortedTexCoords.y+moveOffset);

    vec4 normalMapColor = texture(normalMap, distortedTexCoords);
    //Get normals in view space
    vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b * 8.0, normalMapColor.g * 2.0 - 1.0);
    gNormal = mat3(trans_inv_view)*normalize(normal);
    //gAlbedoSpec = vec4(0.0f, 0.0f, 0.0f, 0.2f);
    gReflect_mask = vec3(0.99f, 0.0f, 0.0f);

    vec2 lastuv=lastUV.xy/lastUV.w;
    lastuv.xy=lastuv.xy*0.5+0.5;
    vec2 curruv=currUV.xy/currUV.w;
    curruv.xy=curruv.xy*0.5+0.5;
	gVelocity = curruv - lastuv;
}