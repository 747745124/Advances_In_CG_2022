#version 330 core
uniform sampler2D gPosition;
uniform sampler2D lastFrame;
uniform sampler2D currFrame;
uniform sampler2D gDepth;
uniform sampler2D gVelocity;


const float alpha=0.1f;
in vec2 aTexCoords;

out vec4 aaColor;

vec2 getClosestVelocity()
{
    vec2 deltaRes = 1.0f/textureSize(gDepth,0).xy;
    float closestDepth = 1.0f;
    vec2 closestUV = aTexCoords;

    for(int i=-1;i<=1;++i)
    {
        for(int j=-1;j<=1;++j)
        {
            vec2 newUV = aTexCoords + deltaRes * vec2(i, j);

            float depth = texture(gDepth, newUV).x;

            if(depth < closestDepth)
            {
                closestDepth = depth;
                closestUV = newUV;
            }
        }
    }
    return closestUV;
}


vec3 RGB2YCoCgR(vec3 rgbColor)
{
    vec3 YCoCgRColor;

    YCoCgRColor.y = rgbColor.r - rgbColor.b;
    float temp = rgbColor.b + YCoCgRColor.y / 2;
    YCoCgRColor.z = rgbColor.g - temp;
    YCoCgRColor.x = temp + YCoCgRColor.z / 2;

    return YCoCgRColor;
}

vec3 YCoCgR2RGB(vec3 YCoCgRColor)
{
    vec3 rgbColor;

    float temp = YCoCgRColor.x - YCoCgRColor.z / 2;
    rgbColor.g = YCoCgRColor.z + temp;
    rgbColor.b = temp - YCoCgRColor.y / 2;
    rgbColor.r = rgbColor.b + YCoCgRColor.y;

    return rgbColor;
}

vec3 clipInYCoCgR(vec3 currColor, vec3 lastColor)
{
    vec3 aabbMin = currColor, aabbMax = currColor;
    vec2 deltaRes = 1.0f/textureSize(currFrame, 0);
    vec3 m1 = vec3(0), m2 = vec3(0);

    for(int i=-1;i<=1;++i)
    {
        for(int j=-1;j<=1;++j)
        {
            vec2 newUV = aTexCoords + deltaRes * vec2(i, j);
            vec3 C = RGB2YCoCgR(texture(currFrame, newUV).rgb);
            m1 += C;
            m2 += C * C;
        }
    }

    // Variance clip
    const int N = 9;
    const float VarianceClipGamma = 1.0f;
    vec3 mu = m1 / N;
    vec3 sigma = sqrt(abs(m2 / N - mu * mu));
    aabbMin = mu - VarianceClipGamma * sigma;
    aabbMax = mu + VarianceClipGamma * sigma;

    // clip to center
    vec3 p_clip = 0.5 * (aabbMax + aabbMin);
    vec3 e_clip = 0.5 * (aabbMax - aabbMin);

    vec3 v_clip = lastColor - p_clip;
    vec3 v_unit = v_clip.xyz / e_clip;
    vec3 a_unit = abs(v_unit);
    float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));

    if (ma_unit > 1.0)
        return p_clip + v_clip / ma_unit;
    else
        return lastColor;
}

void main()
{
    //aaColor=vec4(texture(currFrame,aTexCoords).rgb,1.0f);
    
    vec4 FragPos = texture(gPosition, aTexCoords);
    vec2 lastUV = aTexCoords - texture(gVelocity, getClosestVelocity()).xy;
    vec3 currColor = texture(currFrame,aTexCoords).xyz;
    
    if(lastUV.x<=0.0||lastUV.x>=1.0||lastUV.y<=0.0||lastUV.y>=1.0)
    {
        aaColor = vec4(currColor,1.0f);
        return;
    }
    
    vec3 lastColor = texture(lastFrame,lastUV).xyz;
    if(lastColor.x+lastColor.y+lastColor.z<=0.0f)
    {
        aaColor = vec4(currColor,1.0f);
        return;
    }
    lastColor = clipInYCoCgR(RGB2YCoCgR(currColor), RGB2YCoCgR(lastColor));
    lastColor = YCoCgR2RGB(lastColor);
    aaColor = vec4(lastColor*(1-alpha)+currColor*alpha,1.0f);
    
    
}