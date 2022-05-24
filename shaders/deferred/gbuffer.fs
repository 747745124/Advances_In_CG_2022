#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gMask;
layout (location = 4) out vec2 gVelocity;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;



uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 selected_color;
uniform vec3 viewPos;

in vec4 lastUV;
in vec4 currUV;

void main()
{
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gAlbedoSpec.xyz = texture(texture_diffuse1, TexCoord).rgb;
    gAlbedoSpec.w = texture(texture_specular1, TexCoord).r;
    gMask = vec3(0.0);

    vec2 lastuv=lastUV.xy/lastUV.w;
    lastuv.xy=lastuv.xy*0.5+0.5;
    vec2 curruv=currUV.xy/currUV.w;
    curruv.xy=curruv.xy*0.5+0.5;
	gVelocity = curruv - lastuv;
}