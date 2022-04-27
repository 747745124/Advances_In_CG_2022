#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;



uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform vec3 selected_color;
uniform vec3 viewPos;

void main()
{
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gAlbedoSpec.xyz = texture(texture_diffuse1, TexCoord).rgb;
    gAlbedoSpec.w = texture(texture_specular1, TexCoord).r;

}