#version 330 core
uniform sampler2D rColor;
uniform sampler2D rTexcoord;

in vec2 aTexCoords;
out vec4 FragColor;


void main()
{
    vec4 origin_color = texture(rColor, aTexCoords);

    int scan_size = 5;
    float separation = 2.0;
    vec4 uv = texture(rTexcoord, aTexCoords);
    if(uv.z<=0.0)
    {
        int count=0;
        for(int i=-scan_size;i<=scan_size;i++)
            for(int j=-scan_size;j<=scan_size;j++)
            {
                uv+=texture(rTexcoord, aTexCoords+vec2(i,j)*separation);
                count++;
            }
        uv.xyz/=count;
    }
    if (uv.z <= 0.0) { FragColor = origin_color; return;}
    FragColor = mix(origin_color, texture(rColor, uv.xy), 0.75);
    
}