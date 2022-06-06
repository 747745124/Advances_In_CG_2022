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
uniform vec3 viewPos;
uniform vec3 samples[64];

int kernelSize = 64;
float radius = 0.5;


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
#define strength 1.0
#define PI 3.14159
#define bounceSingularity 1.0
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];


in vec2 aTexCoords;
out vec4 FragColor;

vec3 CalcDirLight(DirLight light, vec3 color, float spec, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = mat3(view)*normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 half_vec = normalize(lightDir+viewDir);
    float specCalc = pow(clamp(dot(half_vec, normal), 0.0f, 1.0f), 256.0f);
    // combine results
    vec3 ambient  = light.ambient  * color;
    vec3 diffuse  = light.diffuse  * diff * color;
    vec3 specular = light.specular * specCalc * spec * color;
    vec3 result = ambient+diffuse+specular;
    return result;
   
}

vec3 CalcPointLight(PointLight light, vec3 color, float spec, vec3 normal, vec3 fragPos, vec3 viewDir)
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
    vec3 ambient = light.ambient * color;
    vec3 diffuse = light.diffuse * diff * color;
    vec3 specular = light.specular * specCalc * spec * color;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return ambient+diffuse+specular;
   
}

void main()
{
    if(enable==0)
    {
        FragColor = vec4(0.0);
        return;
    }
    vec4 FragPos = vec4(texture(gPosition,aTexCoords).xyz,1.0);

    vec3 Color = texture(gAlbedoSpec, aTexCoords).xyz;
    vec3 normal = texture(gNormal,aTexCoords).xyz;
    vec2 texSize = textureSize(gPosition, 0).xy;
    vec2 texNoiseScale = texSize/4.0f;
    float spec = texture(gAlbedoSpec, aTexCoords).w;
    
    vec3 viewDir = normalize(FragPos.xyz);

    if(FragPos.a>0.0){
        vec3 direct = vec3(0.0);
        vec3 occluder = vec3(0.0);
       // vec3 ambient = vec3(0.0);
      //  float ambientOcclusion =0.0;
        vec3 randomSpin = texture(texNoise, aTexCoords*texNoiseScale).xyz;
        //Use schmidt orthogonalization to get a TBN with random tangent
        vec3 tangent = normalize(randomSpin-normal*dot(randomSpin,normal));
        vec3 bitangent = cross(normal, tangent);
        mat3 TBN = mat3(tangent,bitangent,normal);
        //mat3 localMat = computeTripodMatrix(normal);
        //int kernelIndex = int(aTexCoords.x)%kernelSize+(int(aTexCoords.y)%kernelSize*kernelSize);

        for(int i=0;i<kernelSize;i++)
        {
            vec3 bias = TBN*samples[i];
            vec3 sample = FragPos.xyz+bias*radius;
            vec4 sampleScreenCoord = projection*vec4(sample,1.0);
            sampleScreenCoord.xyz /= sampleScreenCoord.w;
           
            sampleScreenCoord.xyz = sampleScreenCoord.xyz*0.5+vec3(0.5);
            
            //vec2 occluderTexCoord = textureSize2D(gPosition,0)*sampleScreenCoord.xy;
            vec2 occluderTexCoord = sampleScreenCoord.xy;
            vec3 occluderColor = texture(gAlbedoSpec, ivec2(occluderTexCoord)).xyz;
            float occluderSpec = texture(gAlbedoSpec, ivec2(occluderTexCoord)).w;
            vec4 occluderPosition =texture(gPosition,ivec2(occluderTexCoord));
            vec3 occluderNormal = texture(gNormal,ivec2(occluderTexCoord)).xyz;
            float depth = sampleScreenCoord.z;
            
            float distanceTerm = abs(sampleScreenCoord.z-occluderPosition.z)<radius?1.0:0.0;
            float visibility = 1.0 - strength *(occluderPosition.z<sampleScreenCoord.z?1.0:0.0)*distanceTerm;//
            vec3 normalizedbias = normalize(bias);
            float receiverGeometricTerm = max(0.0,dot(normal,normalizedbias));

           // float theta = acos(normalizedbias.y);
          //  float phi = atan(normalizedbias.z,normalizedbias.x);
           // if(phi<0.0)phi+=2*PI;
            
           // vec3 senderRadiance = texture(environmentTexture,vec2(phi/(2*PI),1.0-theta/PI)).rgb;
             vec3 senderRadiance = vec3(0.0);
             vec3 directionRadiance = CalcDirLight(dirLight,Color,spec,normal,viewDir);
             senderRadiance = directionRadiance*pow(clamp(0.0f,1.0f,dot(-1.0f*normalizedbias,mat3(view)*normalize(-dirLight.direction))),256.0f);
             for(int i=0;i<NR_POINT_LIGHTS;i++)
              senderRadiance += CalcPointLight(pointLights[i],Color,spec,normal,FragPos.xyz,viewDir)*pow(clamp(0.0f,1.0f,dot(-1.0f*normalizedbias,normalize(vec3(view*vec4(pointLights[i].position,1.0)) - FragPos.xyz))),256.0f); 
            vec3 radiance = visibility * receiverGeometricTerm *senderRadiance;
            //vec3 radiance = receiverGeometricTerm *senderRadiance;
            direct += radiance;



            //vec3 directRadiance = txeture(directRadianceTexture,ivec2(occluderTexCoord));
            //计算遮盖点上接受的直接光
            vec3 directRadiance = directionRadiance;
             for(int i=0;i<NR_POINT_LIGHTS;i++)
              directRadiance+=CalcPointLight(pointLights[i],occluderColor,occluderSpec,occluderNormal,occluderPosition.xyz,viewDir);
            vec3 delta = FragPos.xyz-occluderPosition.xyz;
            vec3 normalizedDelta = normalize(delta);

            float unclampedBounceGeometricTerm =max(0.0,dot(normalizedDelta,-normal))*max(0.0,dot(normalizedDelta,occluderNormal))/dot(delta,delta);
            float bounceGeometricTerm = min(unclampedBounceGeometricTerm,radius);

            vec3 occluderRadiance = directRadiance *bounceGeometricTerm;
            //反射光强度等于反射强度乘以入射到遮盖点的光线强度乘以反射几何项
            occluder += occluderRadiance;
            
        }
        //direct = max(vec3(0.0),direct);
       // occluder = max(vec3(0.0),occluder);

        vec3 radianceSum =direct + occluder;

        radianceSum *=2*PI/kernelSize;
        vec4 mask = texture(gMask, aTexCoords);
        FragColor=(mask.x+mask.y)>0.0f?vec4(0.0):vec4(radianceSum,1.0);
    } 
    else{FragColor=vec4(0.0);}
}