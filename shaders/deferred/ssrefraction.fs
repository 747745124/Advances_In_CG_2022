#version 330 core

in vec2 aTexCoords;

//Position in world space
uniform sampler2D gPosition; 
//Normal in world space
uniform sampler2D gNormal;
//Mask for reflection
uniform sampler2D gMask;

uniform sampler2D gTerrain;

uniform mat4 projection;
uniform mat4 view;//Use this to convert postion from world space to view space


const float refract_index = 1.3f;
const float thickness = 10.0f;

out vec3 refracted_uv;


void main()
{
    //parameters
    float maxDistance = 10;
    float resolution = 0.5;
    int   steps = 4;

    vec2 texSize  = textureSize(gPosition, 0).xy;

    //frag position in view space
    vec4 positionFrom = vec4(texture(gPosition, aTexCoords).xyz, 1.0);
    
    vec4 unitpositionFrom = normalize(positionFrom);
    //get reflection mask
    float mask = texture(gMask, aTexCoords).x;
    
    if(mask<=0.)
    {
        refracted_uv = vec3(0.0);
        return;
    }
    
    //normal in view space
    vec3 normal = normalize(texture(gNormal, aTexCoords).xyz);
    //ray in view space
    vec3 viewRay = positionFrom.xyz;
    //reflect ray in view space
    vec4 pivotView = vec4(refract(viewRay, normal, refract_index), 0.0);
    vec3 pivot = normalize(vec3(pivotView));

    vec4 positionTo = positionFrom;
    //start and end point in view space
    vec4 startView = vec4(positionFrom.xyz, 1.0);
    vec4 endView = vec4(positionFrom.xyz + (pivot * maxDistance), 1.0);

    //convert start and end position from view space to screen space
    vec4 startFrag = projection * startView;
    startFrag.xyz /= startFrag.w;
    startFrag.xyz = startFrag.xyz * 0.5 + 0.5;
    startFrag.xy *= texSize;

    vec4 endFrag = projection * endView;
    endFrag.xyz /= endFrag.w;
    endFrag.xyz = endFrag.xyz * 0.5 + 0.5;
    endFrag.xy *= texSize;

    //DDA algorithm, draw a line on screen space
    float deltaX = endFrag.x - startFrag.x;
    float deltaY = endFrag.y - startFrag.y;
    float useX = abs(deltaX) >= abs(deltaY) ? 1.0 : 0.0;
    float delta = mix(abs(deltaY), abs(deltaX), useX) * clamp(resolution, 0.0, 1.0);
    vec2 increment = vec2(deltaX, deltaY) / max(delta, 0.001);
    float search0 = 0;
    float search1 = 0;

    int hit0 = 0;
    int hit1 = 0;

    float currentZ = startView.z;
    float depth;
    float i = 0;
    vec2 frag = startFrag.xy;
    vec3 uv;
    //linear search pass
    for(; i<int(delta); i++)
    {
        //increment in screen space
        frag += increment;
        uv.xy = frag / texSize;
        if(uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1)
        {
            refracted_uv = vec3(0.0);
            break;
        }
        //get view space position of current pixel
        positionTo = vec4(texture(gTerrain, uv.xy).xyz, 1.0);
        search1 = mix((frag.y-startFrag.y)/deltaY, (frag.x-startFrag.x)/deltaX, useX);
        search1 = clamp(search1, 0., 1.);
        //Perform perspective correct intrapolation
        currentZ = (startView.z * endView.z)/mix(endView.z, startView.z, search1);
        depth =  abs(currentZ)-abs(positionTo.z);
        //check if intersected
        if (depth>0&&depth<thickness) 
        {
            hit0 = 1;
            break;
        } 
        else 
        {
            search0 = search1;
        }
    }

    search1 = search0 + ((search1 - search0) / 2.0);
    steps *= hit0;

    //binary search pass
    for(i=0; i<steps; i++)
    {
        //increment in screen space
        frag = mix(startFrag.xy, endFrag.xy, search1);
        uv.xy = frag / texSize;
        if(uv.x < 0 || uv.x > 1 || uv.y < 0 || uv.y > 1)
        {
            refracted_uv = vec3(0.0);
            break;
        }
        //get view space position of current pixel
        positionTo = vec4(texture(gTerrain, uv.xy).xyz, 1.0);
        //Perform perspective correct intrapolation
        currentZ = (startView.z * endView.z)/mix(endView.z, startView.z, search1);
        depth = abs(currentZ)-abs(positionTo.z);
        //check if intersected
        if (depth>0 && depth<thickness) 
        {
            hit1 = 1;
            search1 = search0 + ((search1 - search0) / 2);
        } 
        else 
        {
            float temp = search1;
            search1 = search1 + ((search1 - search0) / 2);
            search0 = temp;
        }
    }

    float visibility = hit0;

    refracted_uv = vec3(mix(aTexCoords, uv.xy, visibility), 1);
}