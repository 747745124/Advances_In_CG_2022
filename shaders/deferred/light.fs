//Light pass fragment shader
#version 330 core

in vec2 aTexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gMask;
uniform sampler2D SSAOMask;
const int NUM_CASCADES=3;
uniform sampler2D ShadowMap[NUM_CASCADES];
uniform float EndViewSpace[NUM_CASCADES];
uniform mat4 lightMVP[NUM_CASCADES];


layout (location = 0) out vec3 FragColor;

struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 4
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform mat4 view;
uniform mat4 inv_view;

vec3 CalcDirLight(DirLight light, vec3 color, float spec, vec3 normal, vec3 viewDir, float water_mask, float ambient_mask, float shadow)
{
    vec3 lightDir = mat3(view)*normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 half_vec = normalize(lightDir+viewDir);
    float specCalc = pow(clamp(dot(half_vec, normal), 0.0f, 1.0f), 256.0f);
    // combine results
    vec3 ambient  = light.ambient  * color;
    vec3 diffuse  = light.diffuse  * diff * color * water_mask;
    vec3 specular = light.specular * specCalc * spec * color;
    ambient += ambient * (1.0-water_mask) * 1.2;
    specular += specular * (1.0-water_mask) * 2.0;
    return ambient*1.5*pow(ambient_mask,1.5)+diffuse*2.5*shadow+specular;
}

float bias[NUM_CASCADES];

vec3 CalcPointLight(PointLight light, vec3 color, float spec, vec3 normal, vec3 fragPos, vec3 viewDir, float water_mask, float ambient_mask)
{
    vec3 lightDir = normalize(vec3(view*vec4(light.position,1.0)) - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 half_vec = normalize(lightDir+viewDir);
    float specCalc = pow(clamp(dot(half_vec, normal), 0.0f, 1.0f), 256.0f);
    // attenuation
    float distance = length(vec3(view*vec4(light.position,1.0)) - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * color * ambient_mask;
    vec3 diffuse = light.diffuse * diff * color * water_mask;
    vec3 specular = light.specular * specCalc * spec * color;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    ambient += ambient * (1.0-water_mask);
    specular += specular * (1.0-water_mask) * 2.0;
    return (ambient*pow(ambient_mask,1.5)+diffuse)*0.3+specular;
}

float CalcShadow(int index, vec4 PosLightClipSpace)
{
    vec3 screenSpace = PosLightClipSpace.xyz/PosLightClipSpace.w;
    vec2 uv = screenSpace.xy*0.5+0.5;
    float z = screenSpace.z*0.5+0.5;
    vec2 texelSize = 1.0 / textureSize(ShadowMap[index], 0);
    int size=2;
    float occlusion = 0;
    for(int x=-size;x<=size;x++)
        for(int y=-size;y<=size;y++)
        {
            float shadowMapDepth = texture(ShadowMap[index], uv+vec2(x,y)*texelSize).x;
            if(shadowMapDepth+bias[index]<z)
            {
                occlusion+=1;
            }
        }
    return 1-smoothstep(0.0f,1.0f,occlusion/(((size+1)*(size+1))));
}

void main()
{
    vec2 TexCoord = aTexCoords;
    //view space normal
    vec3 Norm = texture(gNormal, TexCoord).xyz;
    //view space fragment position
    vec3 FragPos = texture(gPosition, TexCoord).xyz;
    vec3 Color = texture(gAlbedoSpec, TexCoord).xyz;
    vec3 mask = texture(gMask, TexCoord).rgb;
    float ssao = texture(SSAOMask, TexCoord).r;
    float spec = texture(gAlbedoSpec, TexCoord).w;
    vec3 viewDir = normalize(FragPos);
    float ref_mask = mask.x>0.f?0.:1.;
    //shadow calculation
    bias[0]=0.005f;
    bias[1]=0.005f;
    bias[2]=0.01f;
    vec4 FragPosWorld = inv_view*vec4(FragPos, 1.0f);
    vec4 FragPosLightClipSpace[NUM_CASCADES];
    for(int i=0; i<NUM_CASCADES; i++)
    {
        FragPosLightClipSpace[i] = lightMVP[i] * FragPosWorld;
    }
    float shadow = 1.0f;
    for(int i=0;i<NUM_CASCADES;i++)
    {
        if(abs(FragPos.z)<=EndViewSpace[i])
        {
            shadow = CalcShadow(i,FragPosLightClipSpace[i]);
            break;
        }
    }

    //FragColor=vec3(texture(ShadowMap[1],TexCoord).x);
    //return;
	vec3 result = CalcDirLight(dirLight, Color, spec, Norm, viewDir, ref_mask, ssao, shadow);
	for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], Color, spec, Norm, FragPos, viewDir, ref_mask, ssao);    

	FragColor = (mask.x+mask.y)>=0.1f?Color:result;
}