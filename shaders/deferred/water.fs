#version 330 core


in vec3 FragPos;
in vec2 TexCoords;



layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gReflect_mask;
layout (location = 4) out vec2 gVelocity;

uniform sampler2D normalMap;

const float distStrength = 0.004;

in mat4 trans_inv_view;
in vec4 lastUV;
in vec4 currUV;

void main()
{
    gPosition = FragPos;
    vec3 normalColor=texture(normalMap, TexCoords).xyz;
    vec3 normal = vec3(normalColor.r * 2.0 - 1.0, normalColor.b * 10.0, normalColor.g * 2.0 - 1.0);
    normal = normalize(normal);

    gNormal = mat3(trans_inv_view)*normalize(normal);
    // gNormal = mat3(trans_inv_view)*vec3(0.0,1.0,0.0); //Plane reflection
    //gAlbedoSpec = vec4(0.0f, 0.0f, 0.0f, 0.2f);
    gReflect_mask = vec3(0.2f, 0.0f, 0.0f);

    vec2 lastuv=lastUV.xy/lastUV.w;
    lastuv.xy=lastuv.xy*0.5+0.5;
    vec2 curruv=currUV.xy/currUV.w;
    curruv.xy=curruv.xy*0.5+0.5;
	gVelocity = curruv - lastuv;
}