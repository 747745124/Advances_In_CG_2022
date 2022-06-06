#version 330 core

uniform int enable;
//Position in world space
uniform sampler2D gPosition; 
//Normal in world space
uniform sampler2D gNormal;

uniform sampler2D gAlbedoSpec;

uniform sampler2D gMask;

uniform sampler2D texNoise;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 samples[64];

int kernelSize = 64;
float radius = 0.3;

in vec2 aTexCoords;
layout (location = 0) out vec4 bent_normal_occlude;
layout (location = 1) out vec3 light_bounce;

void main()
{
    //fragpos in view space
    vec4 FragPos = vec4(texture(gPosition, aTexCoords).xyz, 1.0);
    //normal in view space
    vec3 normal = texture(gNormal, aTexCoords).xyz;
    if(enable==0)
    {
        bent_normal_occlude = vec4(normal,1.0);
        light_bounce = vec3(0.0);
        return;
    }
    vec2 texSize = textureSize(gPosition, 0).xy;
    vec2 texNoiseScale = texSize/4.0f;
    vec3 randomSpin = texture(texNoise, aTexCoords*texNoiseScale).xyz;
    //Use schmidt orthogonalization to get a TBN with random tangent
    vec3 tangent = normalize(randomSpin-normal*dot(randomSpin,normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent,bitangent,normal);

    vec3 avg_normal=vec3(0.0);
    vec3 bounced_color=vec3(0.0);
    float count=0;
    float visibility = 0.0;
    for(int i=0;i<kernelSize;i++)
    {
        //view space bias from kernel center
        vec3 bias = TBN*samples[i];
        vec3 sample = vec3(FragPos)+bias*radius;
        vec4 sampleScreenCoord = projection*vec4(sample, 1.0);
        sampleScreenCoord.xyz /= sampleScreenCoord.w;
        sampleScreenCoord.xyz = sampleScreenCoord.xyz*0.5+0.5;
        if(sampleScreenCoord.x<0||sampleScreenCoord.x>1||sampleScreenCoord.y<0||sampleScreenCoord.y>1)
            continue;
        vec3 sampled_position = texture(gPosition, sampleScreenCoord.xy).xyz;
        float sampleScreenZ = sampled_position.z;

        float rangeCheck = smoothstep(0.0,1.0,radius/abs(FragPos.z-sampleScreenZ));
        float vis = abs(sample.z)>=abs(sampleScreenZ)?0:1;
        avg_normal += vis>0?normalize(bias)*rangeCheck:vec3(0.0);
        count += vis;
        visibility += (abs(sample.z)>=abs(sampleScreenZ)?1.0:0.0)*rangeCheck;
        vec3 dir = vec3(FragPos)-sampled_position;

        float cos1 = dot(normalize(dir),texture(gNormal, sampleScreenCoord.xy).xyz);
        cos1=clamp(cos1,0,1);
        float cos2 = dot(normalize(-dir),normal);
        cos2=clamp(cos2,0,1);
        bounced_color += (1-vis)*cos1*cos2*texture(gAlbedoSpec,sampleScreenCoord.xy).xyz;
    }
    visibility = 1.0-visibility/kernelSize;
    //Mask for sky and water, since they don't need to be occluded
    vec4 mask = texture(gMask, aTexCoords);
    bent_normal_occlude.xyz = (count>=1&&(mask.x+mask.y)<=0.f)?normalize(avg_normal):normal;
    bent_normal_occlude.w = (mask.x+mask.y)>0.f?1.0:visibility;;
    light_bounce = (mask.x+mask.y)<=0.f?bounced_color:vec3(0.0);
}