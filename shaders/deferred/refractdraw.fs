#version 330 core
uniform sampler2D gMask;
uniform sampler2D rColor;
uniform sampler2D rTexcoord;
uniform int enable;

in vec2 aTexCoords;
out vec4 FragColor;

void main()
{   
    vec4 origin_color = texture(rColor, aTexCoords);

    if(enable==0)
    {
        FragColor = origin_color;
        return;
    }

    if(texture(gMask, aTexCoords).x<=0.0)//Mask for water
    {
        FragColor=vec4(0.0);
        return;
    }


    int scan_size = 2;
    float separation = 1.0;
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
    
    if (uv.x <= 0.0||uv.y <= 0.0||uv.x > 1.0||uv.y > 1.0)
    { 
        FragColor=vec4(0.0);
        return;
    }
    
    //Use reflected color
    FragColor = texture(rColor, uv.xy);
    
}