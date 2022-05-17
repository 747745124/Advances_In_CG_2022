#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec4 plane;

out vec3 FragPos;

void main()
{
    FragPos = vec3(view * model * vec4(aPos, 1.0f));
    gl_Position = projection * vec4(FragPos, 1.0);
}