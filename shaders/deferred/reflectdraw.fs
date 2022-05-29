#version 330 core
uniform sampler2D rColor;
uniform sampler2D rTexcoord;

uniform samplerCube skybox;
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gMask;

in vec2 aTexCoords;
out vec4 FragColor;

uniform mat4 inv_view;

void main()
{
    vec4 origin_color = texture(rColor, aTexCoords);
    if(texture(gMask, aTexCoords).x<=0.0)//Mask for water
    {
        FragColor=origin_color;
        return;
    }
    int scan_size = 5;
    float separation = 2.0;
    vec4 uv = texture(rTexcoord, aTexCoords);
    
    if(uv.z<=0.0)
    {
        int count=0;
        for(int i=-scan_size;i<=scan_size;i++)
            for(int j=-scan_size;j<=scan_size;j++)
            {
                vec4 sampled_uv=texture(rTexcoord, aTexCoords+vec2(i,j)*separation);
                uv+=sampled_uv;
                count+=(sampled_uv.z>=0.0?1:0);
            }
        count = max(1,count);
        uv.xyz/=count;
    }
    
    if (uv.z <= 0.0)//Perform cubemap lookup
    { 
        vec3 fragpos=texture(gPosition,aTexCoords).xyz;
        vec3 normal=texture(gNormal,aTexCoords).xyz;
        vec3 reflected=reflect(fragpos,normal);
        vec3 reflected_world=vec3(inv_view*vec4(reflected,0.0));
        FragColor=texture(skybox, reflected_world)+vec4(0.0,0.3,0.4,0);
        return;
    }
    //Use reflected color
    FragColor = texture(rColor, uv.xy);
    
}