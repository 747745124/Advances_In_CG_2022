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
    //color = mask>0.1?(color*0.5+vec4(0.0,0.0,0.2,0.5)):color;
    FragColor = mix(color, reflection, mask);
}