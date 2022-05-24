 //Geometry pass fragment shader for terrain
#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gMask;
layout (location = 4) out vec2 gVelocity;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_diffuse1;//blendermap-0
uniform sampler2D texture_diffuse2;//background-1
uniform sampler2D texture_diffuse3;//r-2
uniform sampler2D texture_diffuse4;//g-3
uniform sampler2D texture_diffuse5;//b-4

uniform sampler2D shadowMap;//-5

in vec4 lastUV;
in vec4 currUV;


void main()
{
    vec4 blendermapColour = texture(texture_diffuse1, TexCoord);

    float backgroundAmount = clamp(1 - (blendermapColour.r + blendermapColour.g + blendermapColour.b),0.0,1.0);
    vec2 tiledCoord = TexCoord * 64.0f;
    vec4 backgroundColor = texture(texture_diffuse2, tiledCoord) * backgroundAmount;
    vec4 RColor = texture(texture_diffuse3, tiledCoord) * blendermapColour.r;
    vec4 GColor = texture(texture_diffuse4, tiledCoord) * blendermapColour.g;
    vec4 BColor = texture(texture_diffuse5, tiledCoord) * blendermapColour.b;
    vec4 totalColor = backgroundColor + RColor + GColor + BColor;

    gPosition = FragPos;
    gNormal = Normal;
    gAlbedoSpec.xyz = totalColor.rgb;
    gAlbedoSpec.w = 0.f;
    gMask = vec3(0.0);

    vec2 lastuv=lastUV.xy/lastUV.w;
    lastuv.xy=lastuv.xy*0.5+0.5;
    vec2 curruv=currUV.xy/currUV.w;
    curruv.xy=curruv.xy*0.5+0.5;
	gVelocity = curruv - lastuv;
}