#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform float chunk_size;

const float tiling = 16;

void main()
{
    vec4 World_Pos =  vec4(aPos, 1.0f);
	FragPos = aPos;
    gl_Position = projection * view * World_Pos;
    TexCoord = vec2(aPos.x/chunk_size + 0.5, aPos.z/chunk_size + 0.5) * tiling;
}
