#ifndef _CSM_PCF_SHADOW_RECEIVER_GLSL_
#define _CSM_PCF_SHADOW_RECEIVER_GLSL_

#ifdef CSM_PCF_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX
#define CSM_PCF_SHADOW_RECEIVER_DESCRIPTOR_COUNT 5

#define CASCADE_COUNT 4

layout(set = CSM_PCF_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX + 0, binding = 0) uniform CsmShadowReceiverInfo
{
    float thresholdVZ[CASCADE_COUNT * 2];
    vec3 wLightDirection;
    vec3 vLightDirection;
    float minBias;
    float maxBias;
    mat4 matrixVC2PL[CASCADE_COUNT];
    vec2 texelSize[CASCADE_COUNT];
    int sampleHalfWidth;
}csmShadowReceiverInfo;

layout(set = CSM_PCF_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX + 1, binding = 0) uniform sampler2D shadowTexture_0;
layout(set = CSM_PCF_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX + 2, binding = 0) uniform sampler2D shadowTexture_1;
layout(set = CSM_PCF_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX + 3, binding = 0) uniform sampler2D shadowTexture_2;
layout(set = CSM_PCF_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX + 4, binding = 0) uniform sampler2D shadowTexture_3;

float SampleShadowTexture(int cascadIndex, vec2 aPosition)
{
    float depth;
    switch(cascadIndex)
    {
        case 0:
        {
            depth = texture(shadowTexture_0, aPosition).r;
            break;
        }
        case 1:
        {
            depth = texture(shadowTexture_1, aPosition).r;
            break;
        }
        case 2:
        {
            depth = texture(shadowTexture_2, aPosition).r;
            break;
        }
        case 3:
        {
            depth = texture(shadowTexture_3, aPosition).r;
            break;
        }
    }
    return depth;
}


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
    float shadowIntensity0 = 0;
    float lnDepth0 = 0;
    float shadowIntensity1 = 0;
    float lnDepth1 = 0;

    float bias = 0;
    float cosLightWithNormal = dot(wNormal, -csmShadowReceiverInfo.wLightDirection);
    if(cosLightWithNormal <= 0)
    {
        bias = csmShadowReceiverInfo.minBias;
    }
    else
    {
        bias = max(csmShadowReceiverInfo.maxBias * (1.0 - cosLightWithNormal), csmShadowReceiverInfo.minBias);
    }
    // float bias = 0;
   
    {
        vec4 lpPosition = csmShadowReceiverInfo.matrixVC2PL[(cascadIndex + 1) / 2] * vec4(vPosition, 1);
        vec3 lnPosition = lpPosition.xyz / lpPosition.w;
        lnDepth0 = lnPosition.z;
        vec2 laPosition = (clamp(lnPosition.xy, -1, 1) + vec2(1, -1)) / vec2(2, -2);

        int inShadowCount = 0;
        for(int i = -csmShadowReceiverInfo.sampleHalfWidth; i <= csmShadowReceiverInfo.sampleHalfWidth; i++)
        {
            for(int j = -csmShadowReceiverInfo.sampleHalfWidth; j <= csmShadowReceiverInfo.sampleHalfWidth; j++)
            {
                vec2 offsetAPosition = clamp(laPosition + vec2(i, j) * csmShadowReceiverInfo.texelSize[(cascadIndex + 1) / 2], 0, 1);
                inShadowCount += ((lnDepth0 - bias) > SampleShadowTexture((cascadIndex + 1) / 2, offsetAPosition) ? 1 : 0);
            }
        }
        shadowIntensity0 = float(inShadowCount) / float((csmShadowReceiverInfo.sampleHalfWidth * 2 + 1) * (csmShadowReceiverInfo.sampleHalfWidth * 2 + 1));
        
        // shadowIntensity0 = (lnDepth0 - bias) > SampleShadowTexture((cascadIndex + 1) / 2, laPosition) ? 1 : 0;
    }

    if(cascadIndex % 2 == 0)
    {
        return shadowIntensity0;
    }
    else
    {
        vec4 lpPosition = csmShadowReceiverInfo.matrixVC2PL[cascadIndex / 2] * vec4(vPosition, 1);
        vec3 lnPosition = lpPosition.xyz / lpPosition.w;
        lnDepth1 = lpPosition.z;
        vec2 laPosition = (clamp(lnPosition.xy, -1, 1) + vec2(1, -1)) / vec2(2, -2);

        int inShadowCount = 0;
        for(int i = -csmShadowReceiverInfo.sampleHalfWidth; i <= csmShadowReceiverInfo.sampleHalfWidth; i++)
        {
            for(int j = -csmShadowReceiverInfo.sampleHalfWidth; j <= csmShadowReceiverInfo.sampleHalfWidth; j++)
            {
                vec2 offsetAPosition = clamp(laPosition + vec2(i, j) * csmShadowReceiverInfo.texelSize[cascadIndex / 2], 0, 1);
                inShadowCount += ((lnDepth1 - bias) > SampleShadowTexture(cascadIndex / 2, offsetAPosition) ? 1 : 0);
            }
        }
        shadowIntensity1 = float(inShadowCount) / float((csmShadowReceiverInfo.sampleHalfWidth * 2 + 1) * (csmShadowReceiverInfo.sampleHalfWidth * 2 + 1));
        
        // shadowIntensity1 = (lnDepth1 - bias) > SampleShadowTexture(cascadIndex / 2, laPosition) ? 1 : 0;

        float len = csmShadowReceiverInfo.thresholdVZ[cascadIndex] - csmShadowReceiverInfo.thresholdVZ[cascadIndex + 1];
        float pre = csmShadowReceiverInfo.thresholdVZ[cascadIndex] - vPosition.z;
        float next = vPosition.z - csmShadowReceiverInfo.thresholdVZ[cascadIndex + 1];

        return shadowIntensity0 * pre / len + shadowIntensity1 * next / len;
    }

    // return shadowIntensity0;
}

float GetShadowIntensityWithVNormal(in vec3 vPosition, in vec3 vNormal)
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
    float shadowIntensity0 = 0;
    float lnDepth0 = 0;
    float shadowIntensity1 = 0;
    float lnDepth1 = 0;

    float bias = 0;
    float cosLightWithNormal = dot(vNormal, -csmShadowReceiverInfo.vLightDirection);
    if(cosLightWithNormal <= 0)
    {
        bias = csmShadowReceiverInfo.minBias;
    }
    else
    {
        bias = max(csmShadowReceiverInfo.maxBias * (1.0 - cosLightWithNormal), csmShadowReceiverInfo.minBias);
    }
    // float bias = 0;
    
    {
        vec4 lpPosition = csmShadowReceiverInfo.matrixVC2PL[(cascadIndex + 1) / 2] * vec4(vPosition, 1);
        vec3 lnPosition = lpPosition.xyz / lpPosition.w;
        lnDepth0 = lnPosition.z;
        vec2 laPosition = (clamp(lnPosition.xy, -1, 1) + vec2(1, -1)) / vec2(2, -2);

        int inShadowCount = 0;
        for(int i = -csmShadowReceiverInfo.sampleHalfWidth; i <= csmShadowReceiverInfo.sampleHalfWidth; i++)
        {
            for(int j = -csmShadowReceiverInfo.sampleHalfWidth; j <= csmShadowReceiverInfo.sampleHalfWidth; j++)
            {
                vec2 offsetAPosition = clamp(laPosition + vec2(i, j) * csmShadowReceiverInfo.texelSize[(cascadIndex + 1) / 2], 0, 1);
                inShadowCount += ((lnDepth0 - bias) > SampleShadowTexture((cascadIndex + 1) / 2, offsetAPosition) ? 1 : 0);
            }
        }
        shadowIntensity0 = float(inShadowCount) / float((csmShadowReceiverInfo.sampleHalfWidth * 2 + 1) * (csmShadowReceiverInfo.sampleHalfWidth * 2 + 1));
        
        // shadowIntensity0 = (lnDepth0 - bias) > SampleShadowTexture((cascadIndex + 1) / 2, laPosition) ? 1 : 0;
    }

    if(cascadIndex % 2 == 0)
    {
        return shadowIntensity0;
    }
    else
    {
        vec4 lpPosition = csmShadowReceiverInfo.matrixVC2PL[cascadIndex / 2] * vec4(vPosition, 1);
        vec3 lnPosition = lpPosition.xyz / lpPosition.w;
        lnDepth1 = lpPosition.z;
        vec2 laPosition = (clamp(lnPosition.xy, -1, 1) + vec2(1, -1)) / vec2(2, -2);

        int inShadowCount = 0;
        for(int i = -csmShadowReceiverInfo.sampleHalfWidth; i <= csmShadowReceiverInfo.sampleHalfWidth; i++)
        {
            for(int j = -csmShadowReceiverInfo.sampleHalfWidth; j <= csmShadowReceiverInfo.sampleHalfWidth; j++)
            {
                vec2 offsetAPosition = clamp(laPosition + vec2(i, j) * csmShadowReceiverInfo.texelSize[cascadIndex / 2], 0, 1);
                inShadowCount += ((lnDepth1 - bias) > SampleShadowTexture(cascadIndex / 2, offsetAPosition) ? 1 : 0);
            }
        }
        shadowIntensity1 = float(inShadowCount) / float((csmShadowReceiverInfo.sampleHalfWidth * 2 + 1) * (csmShadowReceiverInfo.sampleHalfWidth * 2 + 1));
        
        // shadowIntensity1 = (lnDepth1 - bias) > SampleShadowTexture(cascadIndex / 2, laPosition) ? 1 : 0;

        float len = csmShadowReceiverInfo.thresholdVZ[cascadIndex] - csmShadowReceiverInfo.thresholdVZ[cascadIndex + 1];
        float pre = csmShadowReceiverInfo.thresholdVZ[cascadIndex] - vPosition.z;
        float next = vPosition.z - csmShadowReceiverInfo.thresholdVZ[cascadIndex + 1];

        return shadowIntensity0 * pre / len + shadowIntensity1 * next / len;
    }

    // return shadowIntensity0;
}

#endif ///#ifdef CSM_PCF_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX

#endif ///#ifndef _CSM_PCF_SHADOW_RECEIVER_GLSL_