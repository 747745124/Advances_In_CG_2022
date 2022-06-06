#version 330 core
out vec4 color;
in vec2 TexCoords;

uniform sampler2D postBuffer;
uniform float weight[9] = float[] (1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0);
uniform float exposure;
uniform int toneMapping;
uniform int effectType; //0 for sobel, -1 for none, 1 for DoF
const float gamma=2.2;


float minDistance = 1.0;
float maxDistance = 3.0;

void main()
{             
    if(toneMapping==1)
    {
        vec3 hdrColor = texture(postBuffer, TexCoords).rgb;

        // reinhard
        // exposure
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
        // also gamma correct while we're at it       
        result = pow(result, vec3(1.0 / gamma));
        color = vec4(result, 1.0);
        return;
    }
    

    if(effectType==0)
    {
        //doing sobel detection
        vec4 tex = texture(postBuffer, TexCoords);

        vec4 result = tex;

        vec2 texelSize = 1.0 / vec2(textureSize(postBuffer, 0));
        result = vec4(0.0,0.0,0.0,1.0);

        int i = 0;
        for (int x = -1; x <= 1; ++x) 
        {
            for (int y = 1; y >= -1; --y) 
            {
                vec2 offset = vec2(float(x), float(y)) * texelSize;
                vec4 tmp = texture(postBuffer, TexCoords + offset).rgba;
                tmp = vec4(vec3((tmp.r + tmp.g + tmp.b) /3.0), 1.0);
                result += tmp * weight[i++];
            }
        }

        i = 0;
        
        for (int y = 1; y >= -1; --y) 
        {
            for (int x = -1; x <= 1; ++x) 
            {
                vec2 offset = vec2(float(x), float(y)) * texelSize;
                vec4 tmp = texture(postBuffer, TexCoords + offset).rgba;
                tmp = vec4(vec3((tmp.r + tmp.g + tmp.b) /3.0), 1.0);
                result += tmp * weight[i++];
            }
        }

        color = result;
        return;
    }
    else if(effectType==1){
        //doing DoF, post processing



    }
    else{
        color = texture(postBuffer, TexCoords);
        return;
    }
}