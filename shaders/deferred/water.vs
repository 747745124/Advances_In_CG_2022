#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 jitteredProjection;
uniform mat4 lastViewProjection;
uniform float chunk_size;

uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform float moveOffset;

out mat4 trans_inv_view;


const float tiling = 8;

out vec4 lastUV;
out vec4 currUV;
out vec2 TexCoords;

void main()
{
    vec2 TexCoord = vec2(aPos.x/chunk_size + 0.5, aPos.z/chunk_size + 0.5) * tiling;
    vec2 distortedTexCoords = texture(dudvMap, vec2(TexCoord.x+moveOffset, TexCoord.y)).rg*0.1;
	distortedTexCoords = TexCoord + vec2(distortedTexCoords.x, distortedTexCoords.y+moveOffset);
    TexCoords=distortedTexCoords;

    vec3 normalColor = texture(normalMap, distortedTexCoords).xyz;
    vec3 normal = vec3(normalColor.r * 2.0 - 1.0, normalColor.b * 3.0, normalColor.g * 2.0 - 1.0);
    normal = normalize(normal);

    vec4 World_Pos =  vec4(aPos, 1.0f);
    FragPos = vec3(view * World_Pos);
    
    gl_Position = jitteredProjection * view * World_Pos;

    trans_inv_view = transpose(inverse(view));

    lastUV = lastViewProjection * World_Pos;
    currUV = projection * view * World_Pos;
}
