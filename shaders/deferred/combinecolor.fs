#version 330 core
in vec2 aTexCoords;

uniform sampler2D Tcolor;
uniform sampler2D Treflection;
uniform sampler2D gMask;

out vec4 FragColor;

void main()
{
    float mask = texture(gMask, aTexCoords).x;
    vec4 color = texture(Tcolor, aTexCoords);
    vec4 reflection = texture(Treflection, aTexCoords);
    reflection = (reflection.r+reflection.g+reflection.b)<=0.1?color:reflection;
    FragColor = mix(color, reflection, mask);
}