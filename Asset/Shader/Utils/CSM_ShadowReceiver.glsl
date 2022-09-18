#ifndef _CSM_SHADOW_RECEIVER_GLSL_
#define _CSM_SHADOW_RECEIVER_GLSL_

#ifdef CSM_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX
#define CSM_SHADOW_RECEIVER_DESCRIPTOR_COUNT 5

#define CASCADE_COUNT 4

layout(set = CSM_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX + 0, binding = 0) uniform CsmShadowReceiverInfo
{
    float thresholdVZ[CASCADE_COUNT + 1];
    vec3 wLightDirection;
    float minBias;
    float maxBias;
    mat4 matrixVC2PL[CASCADE_COUNT];
}csmShadowReceiverInfo;

layout(set = CSM_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX + 1, binding = 0) uniform sampler2D shadowTexture_0;
layout(set = CSM_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX + 2, binding = 0) uniform sampler2D shadowTexture_1;
layout(set = CSM_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX + 3, binding = 0) uniform sampler2D shadowTexture_2;
layout(set = CSM_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX + 4, binding = 0) uniform sampler2D shadowTexture_3;

float GetShadowIntensity(in vec3 vPosition, in vec3 wNormal)
{
    int cascadIndex = -1;
    for(int i = 0; i < CASCADE_COUNT + 1; i++)
    {
        if(vPosition.z > csmShadowReceiverInfo.thresholdVZ[i])
        {
            cascadIndex = i - 1;
            break;
        }
    }

    if(cascadIndex == -1) return 1;

    vec4 lvPosition = csmShadowReceiverInfo.matrixVC2PL[cascadIndex] * vec4(vPosition, 1);
    vec3 lnPosition = lvPosition.xyz / lvPosition.w;
    vec2 laPosition = (clamp(lvPosition.xy, -1, 1) + vec2(1, -1)) / vec2(2, -2);
    float lnVisiableDepth = 0;
    switch(cascadIndex)
    {
        case 0:
        {
            lnVisiableDepth = texture(shadowTexture_0, laPosition).r;
            break;
        }
        case 1:
        {
            lnVisiableDepth = texture(shadowTexture_1, laPosition).r;
            break;
        }
        case 2:
        {
            lnVisiableDepth = texture(shadowTexture_2, laPosition).r;
            break;
        }
        case 3:
        {
            lnVisiableDepth = texture(shadowTexture_3, laPosition).r;
            break;
        }
    }
    float bias = max(csmShadowReceiverInfo.maxBias * (1.0 - dot(wNormal, -csmShadowReceiverInfo.wLightDirection)), csmShadowReceiverInfo.minBias);
    // return lnVisiableDepth < lnPosition.z ? 1.0f : 0.0f;
    return lnPosition.z - bias > lnVisiableDepth ? 1.0 : 0.0;
}

#endif ///#ifdef CSM_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX

#endif ///#ifndef _CSM_SHADOW_RECEIVER_GLSL_