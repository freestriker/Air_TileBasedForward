#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"
#include "Object.glsl"
#include "Light.glsl"

layout(location = 0) in vec3 inWorldPosition;
layout(location = 1) in vec3 inWorldNormal;

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
struct SubGerstnerWaveInfo
{
    float amplitudeFactor;
    float windRotationAngle;
    float waveLength;
    float phiAngle;
};
layout(set = 2, binding = 0) readonly buffer SubGerstnerWaveInfosBuffer
{
    SubGerstnerWaveInfo subGerstnerWaveInfos[];
}subGerstnerWaveInfosBuffer;

layout(push_constant) uniform ProjectedGridConstantInfo
{
    vec4 corner00;
    vec4 corner10;
    vec4 corner01;
    vec4 corner11;
    uint count;
    float time;
} constantInfo;

void main() 
{
    const vec3 normal = normalize(inWorldNormal);
    const float bubbles = 1;

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
}
