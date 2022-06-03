#version 330 core
in vec2 aTexCoords;

uniform sampler2D Tcolor;
uniform sampler2D Treflection;
uniform sampler2D Trefraction;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gMask;


out vec4 FragColor;


void main()
{
    vec3 normal=normalize(texture(gNormal, aTexCoords).xyz);
    vec3 view=normalize(-texture(gPosition, aTexCoords).xyz);
    float coeff=clamp(dot(normal,view),0.0,1.0);
    float mask = texture(gMask, aTexCoords).x;
    vec4 color = texture(Tcolor, aTexCoords);
    vec4 reflection = texture(Treflection, aTexCoords);
    vec4 refraction = texture(Trefraction, aTexCoords);
    vec4 water = mix(reflection,refraction,coeff);
    FragColor = mask<=0.0?color:water;
}