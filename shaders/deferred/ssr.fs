#version 330 core

//Position in world space
uniform sampler2D gPosition; 
//Normal in world space
uniform sampler2D gNormal;
//Mask for reflection
uniform sampler2D gReflect_mask;

uniform mat4 projection;
uniform mat4 view;//Use this to convert postion from world space to view space
uniform vec3 viewPos;

in vec2 aTexCoords;

layout (location = 1) out vec3 reflected_uv;

void main()
{
    //parameters
    float maxDistance = 500;
    float resolution  = 0.8;
    int   steps       = 16;
    float thickness   = 30;

    vec2 texSize  = textureSize(gPosition, 0).xy;

    //frag position in world space
    vec4 positionWorld = vec4(texture(gPosition, aTexCoords).xyz, 1.0);
    //convert to view space
    vec4 positionFrom = view * positionWorld;
    //get reflection mask
    float mask = texture(gReflect_mask, aTexCoords).x;

    if(mask<=0.)
    {
        reflected_uv = vec3(0.0);
        return;
    }

    //normal in world space
    vec3 normal = normalize(texture(gNormal, aTexCoords).xyz);
    //ray in world space
    vec3 viewRay = positionWorld.xyz-viewPos;
    //reflect ray in world space
    vec4 pivotWorld = vec4(reflect(viewRay, normal), 0.0);
    //convert reflected ray to view space
    vec3 pivot = normalize(vec3(view * pivotWorld));

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
    float depth = thickness;
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
            reflected_uv = vec3(0.0);
            break;
        }
        //get view space position of current pixel
        positionTo = view * vec4(texture(gPosition, uv.xy).xyz, 1.0);
        search1 = mix((frag.y-startFrag.y)/deltaY, (frag.x-startFrag.x)/deltaX, useX);
        search1 = clamp(search1, 0., 1.);
        //Perform perspective correct intrapolation
        currentZ = (startView.z * endView.z)/mix(endView.z, startView.z, search1);
        depth = abs(currentZ) - abs(positionTo.z);
        //check if intersected
        if (depth>0 && depth< thickness) 
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
            reflected_uv = vec3(0.0);
            break;
        }
        //get view space position of current pixel
        positionTo = view * vec4(texture(gPosition, uv.xy).xyz, 1.0);
        //Perform perspective correct intrapolation
        currentZ = (startView.z * endView.z)/mix(endView.z, startView.z, search1);
        depth = abs(currentZ) - abs(positionTo.z);
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

    float visibility = hit1 * positionTo.w
    * (1 - clamp(depth/thickness, 0, 1))
    * (1 - clamp(length(positionTo - positionFrom)/ maxDistance, 0, 1));

    reflected_uv = vec3(uv.xy,visibility);
}