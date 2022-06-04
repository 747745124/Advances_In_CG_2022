#version 330 core

in vec3 FragPos;
out vec3 gPosition;

void main()
{
    gPosition=FragPos;
}