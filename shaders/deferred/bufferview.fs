#version 330 core

uniform sampler2D input_buffer;
in vec2 aTexCoords;
out vec4 FragColor;

void main()
{
    FragColor = vec4(texture(input_buffer,aTexCoords).x);

}