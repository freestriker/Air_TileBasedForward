#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"
#include "Object.glsl"
#include "Light.glsl"

layout(location = 0) in vec2 inTexCoords;
layout(location = 1) in vec3 inWorldPosition;

layout(location = 0) out vec4 ColorAttachment;

layout(set = 0, binding = 0) uniform _CameraInfo
{
    CameraInfo info;
} cameraInfo;
#define MAX_ORTHER_LIGHT_COUNT 256
layout(set = 1, binding = 0) uniform LightInfos
{
    LightInfo ambientLightInfo;
    LightInfo mainLightInfo;
    int ortherLightCount;
    LightInfo[MAX_ORTHER_LIGHT_COUNT] ortherLightInfos;
} lightInfos;
layout(set = 2, binding = 0) uniform sampler2D displacementTexture;
layout(set = 3, binding = 0) uniform sampler2D normalTexture;

layout(push_constant) uniform ProjectedGridConstantInfo
{
    vec4 corner00;
    vec4 corner10;
    vec4 corner01;
    vec4 corner11;
    vec3 scale;
} constantInfo;

void main() 
{
    // vec2 texCoords;
    // {
    //     const vec2 offset = mod(inWorldPosition.xz, constantInfo.scale.xz);
    //     texCoords = offset / constantInfo.scale.xz;
    //     texCoords.x = 1 - texCoords.x;
    // }
    // const vec2 inTexCoords = texCoords;
    // ivec2 ij = ivec2(inWorldPosition.xz / constantInfo.scale.xz);
    // if(!(ij.x == 0 && ij.y == 0)) discard;

    const vec4 normal_bubbles = texture(normalTexture, inTexCoords).rgba;
    const vec3 normal = normalize(normal_bubbles.xyz);
    const float bubbles = normal_bubbles.w;

    vec3 Ci;
    {
        vec3 upwelling = vec3(0, 0.2, 0.3);
        vec3 sky = vec3(0.69,0.84,1);
        vec3 air = vec3(0.1,0.1,0.1);
        float nSnell = 1.34;
        float Kdiffuse = 0.91;
   
        vec3 nI = normalize(-lightInfos.mainLightInfo.direction);
        vec3 nN = normalize(normal);
        float costhetai = abs(dot(nI, nN));
        float thetai = acos(costhetai);
        float sinthetat = sin(thetai) / nSnell;
        float thetat = asin(sinthetat);
        float reflectivity;
        if(thetai == 0.0)
        {
            reflectivity = (nSnell - 1) / (nSnell + 1);
            reflectivity = reflectivity * reflectivity;
        }
        else
        {
            float fs = sin(thetat - thetai) / sin(thetat + thetai);
            float ts = tan(thetat - thetai) / tan(thetat + thetai);
            reflectivity = 0.5 * (fs * fs + ts * ts);
        }
        float dist = exp(-length(inWorldPosition - cameraInfo.info.position) * Kdiffuse);
        Ci = /*dist * */(reflectivity * sky + (1 - reflectivity) * upwelling) + (1 - dist) * air;
    }

    ColorAttachment = vec4(Ci + vec3(bubbles), 1);
    // ColorAttachment = vec4(inTexCoords, 1, 1);
    // ColorAttachment = vec4(mod(inWorldPosition.xz, constantInfo.scale.xz) / constantInfo.scale.xz, 0, 1);
}
