#version 330 core

in vec3 beforeRefract;
in vec3 afterRefract;

out vec4 FragColor;

uniform sampler2D terrainDepth;

const float caustics_factor=0.20;

void main()
{
    float areaBeforeRefract = length(dFdx(beforeRefract.xy))*length(dFdy(beforeRefract.xy));
    float areaAfterRefract = length(dFdx(afterRefract.xy))*length(dFdy(afterRefract.xy));
    float ratio;
    if(areaAfterRefract<=0.0)
    {
        ratio=1.0/0.00001;
    }
    else
    {
        ratio=areaBeforeRefract/areaAfterRefract;
    }

    FragColor=vec4(caustics_factor*vec3(ratio),1.0);
    //FragColor.x=texture(terrainDepth,gl_FragCoord.xy/textureSize(terrainDepth,0).xy).x;
}