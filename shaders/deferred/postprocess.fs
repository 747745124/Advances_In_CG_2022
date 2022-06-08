#version 400 core
out vec4 color;
in vec2 TexCoords;

uniform sampler2D postBuffer;
//sobel kernel
uniform float weight[9] = float[](1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0);
uniform float exposure;
uniform int toneMapping;
uniform int effectType; //0 for sobel, -1 for none, 1 for DoF
const float gamma = 2.2;

const int step = 10;
const int mean_step = 7;
uniform float[441] weights;
uniform vec2 mouseFocus;

float minDistance = 0.0;
float maxDistance = 0.707f;//sqrt(2)

void main() {
    if(toneMapping == 1) {
        vec3 hdrColor = texture(postBuffer, TexCoords).rgb;

        // reinhard
        // exposure
        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
        // also gamma correct while we're at it       
        result = pow(result, vec3(1.0 / gamma));
        color = vec4(result, 1.0);
        return;
    }

    if(effectType == 0) {
        //doing sobel detection
        vec4 tex = texture(postBuffer, TexCoords);

        vec4 result = tex;

        vec2 texelSize = 1.0 / vec2(textureSize(postBuffer, 0));
        result = vec4(0.0, 0.0, 0.0, 1.0);

        int i = 0;

        for(int x = -1; x <= 1; ++x) {
            for(int y = 1; y >= -1; --y) {
                vec2 offset = vec2(float(x), float(y)) * texelSize;
                vec4 tmp = texture(postBuffer, TexCoords + offset).rgba;
                tmp = vec4(vec3((tmp.r + tmp.g + tmp.b) / 3.0), 1.0);
                result += tmp * weight[i++];
            }
        }

        i = 0;

        for(int y = 1; y >= -1; --y) {
            for(int x = -1; x <= 1; ++x) {
                vec2 offset = vec2(float(x), float(y)) * texelSize;
                vec4 tmp = texture(postBuffer, TexCoords + offset).rgba;
                tmp = vec4(vec3((tmp.r + tmp.g + tmp.b) / 3.0), 1.0);
                result += tmp * weight[i++];
            }
        }

        color = result;
        return;

    } else if(effectType == 1) {

        vec4 fragColor = texture(postBuffer, TexCoords);
        color = fragColor;

        vec4 tempColor = vec4(0.0, 0.0, 0.0, 1.0);
        float count = 0.0;

        int i = 0;

        for(int x = -step; x <= step; ++x) {
            for(int y = -step; y <= step; ++y) {
                vec2 offset = vec2(float(x), float(y)) * 1.0 / vec2(textureSize(postBuffer, 0));
                vec4 tmp = texture(postBuffer, TexCoords + offset);
                tempColor.rgb += tmp.rgb * weights[i++];
            }
        }
        vec4 blurColor = vec4(tempColor.rgb, 1.0f);

        tempColor.rgb = vec3(0.0);
        for(int x = -mean_step; x <= mean_step; ++x) {
            for(int y = -mean_step; y <= mean_step; ++y) {
                vec2 offset = vec2(float(x), float(y)) * 1.0 / vec2(textureSize(postBuffer, 0));
                vec4 tmp = texture(postBuffer, TexCoords + offset);
                tempColor.rgb += tmp.rgb;
            }
        }

        vec4 mean_color = vec4(tempColor.rgb / pow((mean_step * 2 + 1), 2), 1.0);

        blurColor = mix(blurColor, mean_color, 0.8);

        float blur = smoothstep(minDistance, maxDistance, length(TexCoords - mouseFocus));

        // color = vec4(blur, blur, blur, 1.0f);
        color = mix(fragColor, blurColor, blur);
        return;

    } else {
        color = texture(postBuffer, TexCoords);
        return;
    }
}