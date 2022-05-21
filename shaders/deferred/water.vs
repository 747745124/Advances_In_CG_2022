#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform float chunk_size;

out mat4 trans_inv_view;

const float tiling = 64;

void main()
{
    vec4 World_Pos =  vec4(aPos, 1.0f);
	FragPos = vec3(view * World_Pos);
    gl_Position = projection * view * World_Pos;
    trans_inv_view = transpose(inverse(view));
    TexCoord = vec2(aPos.x/chunk_size + 0.5, aPos.z/chunk_size + 0.5) * tiling;
}
