#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform sampler2D terrainDepth;
uniform float moveOffset;
uniform float chunk_size;
uniform vec3 lightDir;

const float refract_index = 0.77;
const float max_distance = 5;
const float resolution = 0.5;

const float tiling = 8;

out vec3 beforeRefract;
out vec3 afterRefract;

void main()
{
    vec2 TexCoord = vec2(aPos.x/chunk_size + 0.5, aPos.z/chunk_size + 0.5) * tiling;
    vec2 distortedTexCoords = texture(dudvMap, vec2(TexCoord.x+moveOffset, TexCoord.y)).rg*0.1;
	distortedTexCoords = TexCoord + vec2(distortedTexCoords.x, distortedTexCoords.y+moveOffset);

    vec3 normalColor = texture(normalMap, distortedTexCoords).xyz;
    vec3 normal = vec3(normalColor.r * 2.0 - 1.0, normalColor.b * 0.4, normalColor.g * 2.0 - 1.0);
    normal = normalize(normal);

    vec4 World_Pos = vec4(aPos, 1.0f);
    
    gl_Position = projection * view * World_Pos;

    vec3 refracted = refract(lightDir,normal,refract_index);
    refracted = normalize(refracted);

    vec2 texSize = textureSize(terrainDepth,0).xy;

    vec3 startPosition = vec3(aPos.x,aPos.y+0.1f,aPos.z);
    
    vec4 startPositionScreen = projection*view*vec4(startPosition,1.0f);
    startPositionScreen.xyz /= startPositionScreen.w;
    startPositionScreen.xyz = startPositionScreen.xyz*0.5+0.5;
    startPositionScreen.xy *= texSize;
    beforeRefract=startPositionScreen.xyz;
    

    vec3 endPosition = startPosition+refracted*max_distance;
    vec4 endPositionScreen = projection*view*vec4(endPosition,1.0f);
    endPositionScreen.xyz /= endPositionScreen.w;
    endPositionScreen.xyz = endPositionScreen.xyz*0.5+0.5;
    endPositionScreen.xy *= texSize;

    float deltaX = endPositionScreen.x - startPositionScreen.x;
    float deltaY = endPositionScreen.y - startPositionScreen.y;
    if(abs(deltaX)<0.1||abs(deltaY)<0.1)
    {
        afterRefract=startPositionScreen.xyz;
        return;
    }
    float useX = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
    float delta = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0.0, 1.0);
    vec2 increment = vec2(deltaX, deltaY) / max(delta, 0.001);
    
    vec2 frag=startPositionScreen.xy;
    float hit=0.;
    const float MAX_ITER=30.0;
    for(float i=0;i<min(delta,MAX_ITER);i++)
    {
        frag += increment;
        vec2 uv = frag/texSize;
        if(uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1)
        {
            afterRefract=startPositionScreen.xyz;
            break;
        }
        float depthOnMap = texture(terrainDepth,uv).x;
        float alpha = mix((frag.y-startPositionScreen.y)/deltaY, (frag.x-startPositionScreen.x)/deltaX, useX);
        alpha = clamp(alpha,0.,1.);
        float currDepth = (startPositionScreen.z * endPositionScreen.z)/mix(endPositionScreen.z, startPositionScreen.z, alpha);
        float depth = currDepth-depthOnMap;
        if(depth>0)
        {
            hit=1;
            afterRefract=startPositionScreen.xyz+(endPositionScreen.xyz-startPositionScreen.xyz)*alpha;
            break;
        }
    }
    if(hit<1.0)
    {
        afterRefract=startPositionScreen.xyz;
    }

}