#ifndef _CSM_SHADOW_RECEIVER_GLSL_
#define _CSM_SHADOW_RECEIVER_GLSL_

#ifdef CSM_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX
#define CSM_SHADOW_RECEIVER_DESCRIPTOR_COUNT 5

#define CASCADE_COUNT 4

layout(set = CSM_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX + 0, binding = 0) uniform CsmShadowReceiverInfo
{
    float thresholdVZ[CASCADE_COUNT * 2];
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
    for(int i = 0; i < CASCADE_COUNT * 2; i++)
    {
        if(vPosition.z > csmShadowReceiverInfo.thresholdVZ[i])
        {
            cascadIndex = i - 1;
            break;
        }
    }

    if(cascadIndex == -1) return 1;
    float lnVisiableDepth0 = 0;
    float lnDepth0 = 0;
    float lnVisiableDepth1 = 0;
    float lnDepth1 = 0;

    float bias = max(csmShadowReceiverInfo.maxBias * (1.0 - dot(wNormal, -csmShadowReceiverInfo.wLightDirection)), csmShadowReceiverInfo.minBias);
    
    {
        vec4 lvPosition = csmShadowReceiverInfo.matrixVC2PL[(cascadIndex + 1) / 2] * vec4(vPosition, 1);
        vec3 lnPosition = lvPosition.xyz / lvPosition.w;
        lnDepth0 = lnPosition.z;
        vec2 laPosition = (clamp(lvPosition.xy, -1, 1) + vec2(1, -1)) / vec2(2, -2);
        switch((cascadIndex + 1) / 2)
        {
            case 0:
            {
                lnVisiableDepth0 = texture(shadowTexture_0, laPosition).r;
                break;
            }
            case 1:
            {
                lnVisiableDepth0 = texture(shadowTexture_1, laPosition).r;
                break;
            }
            case 2:
            {
                lnVisiableDepth0 = texture(shadowTexture_2, laPosition).r;
                break;
            }
            case 3:
            {
                lnVisiableDepth0 = texture(shadowTexture_3, laPosition).r;
                break;
            }
        }
    }
    if(cascadIndex % 2 == 0)
    {
        return lnDepth0 - bias > lnVisiableDepth0 ? 1.0 : 0.0;
    }
    else
    {
        vec4 lvPosition = csmShadowReceiverInfo.matrixVC2PL[cascadIndex / 2] * vec4(vPosition, 1);
        vec3 lnPosition = lvPosition.xyz / lvPosition.w;
        lnDepth1 = lnPosition.z;
        vec2 laPosition = (clamp(lvPosition.xy, -1, 1) + vec2(1, -1)) / vec2(2, -2);
        switch(cascadIndex / 2)
        {
            case 0:
            {
                lnVisiableDepth1 = texture(shadowTexture_0, laPosition).r;
                break;
            }
            case 1:
            {
                lnVisiableDepth1 = texture(shadowTexture_1, laPosition).r;
                break;
            }
            case 2:
            {
                lnVisiableDepth1 = texture(shadowTexture_2, laPosition).r;
                break;
            }
            case 3:
            {
                lnVisiableDepth1 = texture(shadowTexture_3, laPosition).r;
                break;
            }
        }

        return lnDepth0 - bias > lnVisiableDepth0 || lnDepth1 - bias > lnVisiableDepth1 ? 1.0 : 0.0;
    }

    // return lnDepth0 - bias > lnVisiableDepth0 ? 1.0 : 0.0;
}

#endif ///#ifdef CSM_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX

#endif ///#ifndef _CSM_SHADOW_RECEIVER_GLSL_