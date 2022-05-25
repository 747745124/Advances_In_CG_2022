#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 jitteredProjection;
uniform mat4 lastViewProjection;

out vec3 TexCoord;
out vec3 FragPos;

out vec4 lastUV;
out vec4 currUV;

void main()
{
    vec4 World_Pos = model * vec4(aPos, 1.0f);
    FragPos = vec3(view * World_Pos);
    gl_Position = jitteredProjection * view * World_Pos;

    lastUV = lastViewProjection * World_Pos;
    currUV = projection * view * World_Pos;

    TexCoord = aPos;
}