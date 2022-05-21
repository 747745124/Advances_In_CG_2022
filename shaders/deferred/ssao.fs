#version 330 core

//Position in world space
uniform sampler2D gPosition; 
//Normal in world space
uniform sampler2D gNormal;

uniform sampler2D gMask;

uniform sampler2D texNoise;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 samples[64];

int kernelSize = 64;
float radius = 0.3;

in vec2 aTexCoords;
out float FragColor;

void main()
{
    //fragpos in view space
    vec4 FragPos = vec4(texture(gPosition, aTexCoords).xyz, 1.0);
    //normal in view space
    vec3 normal = texture(gNormal, aTexCoords).xyz;
    vec2 texSize = textureSize(gPosition, 0).xy;
    vec2 texNoiseScale = texSize/4.0f;
    vec3 randomSpin = texture(texNoise, aTexCoords*texNoiseScale).xyz;
    //Use schmidt orthogonalization to get a TBN with random tangent
    vec3 tangent = normalize(randomSpin-normal*dot(randomSpin,normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent,bitangent,normal);

    float visibility = 0.0;
    for(int i=0;i<kernelSize;i++)
    {
        //view space bias from kernel center
        vec3 bias = TBN*samples[i];
        vec3 sample = vec3(FragPos)+bias*radius;
        vec4 sampleScreenCoord = projection*vec4(sample, 1.0);
        sampleScreenCoord.xyz /= sampleScreenCoord.w;
        sampleScreenCoord.xyz = sampleScreenCoord.xyz*0.5+0.5;
        float sampleScreenZ = vec4(texture(gPosition, sampleScreenCoord.xy).xyz,1.0).z;

        float rangeCheck = smoothstep(0.0,1.0,radius/abs(FragPos.z-sampleScreenZ));
        visibility += (abs(sample.z)>=abs(sampleScreenZ)?1.0:0.0)*rangeCheck;
    }
    visibility = 1.0-visibility/kernelSize;
    //Mask for sky and water, since they don't need to be occluded
    vec4 mask = texture(gMask, aTexCoords);
    FragColor = (mask.x+mask.y)>0.f?1.0:visibility;
}