#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;

void main()
{
    vec4 World_Pos = model * vec4(aPos, 1.0f);
    FragPos = vec3(view * World_Pos);
    gl_Position = projection * vec4(FragPos,1.0f);
}