#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"
#include "Object.glsl"

#define PI (acos(-1.0))
#define DOUBLE_PI (2 * acos(-1.0))

layout(set = 0, binding = 0) uniform _CameraInfo
{
    CameraInfo info;
} cameraInfo;
layout(set = 1, binding = 0) uniform sampler2D noiseTexture;
layout(set = 2, binding = 0) uniform sampler2D depthTexture;
layout(set = 3, binding = 0) uniform HbaoInfo
{
    vec2 attachmentSize;
    vec2 attachmentTexelSize;
    float sampleRadius;
    float sampleBiasAngle;
    int stepCount;
    int directionCount;
    int noiseTextureWidth;
} hbaoInfo;

layout(location = 0) out float OcclusionTexture;

void main()
{
    vec2 aPosition = gl_FragCoord.xy * hbaoInfo.attachmentTexelSize;
    float ndcDepth =  texture(depthTexture, aPosition).r;
    vec3 ndcPosition = vec3(PositionA2N(aPosition), ndcDepth);
    vec3 vPosition = PositionN2V(ndcPosition, cameraInfo.info);
    float vDepth = vPosition.z;

    vec3 faceVNormal;
    {
        vec2 lAPosition = clamp((gl_FragCoord.xy + vec2(-1, 0)) * hbaoInfo.attachmentTexelSize, 0, 1);
        float lNdcDepth =  texture(depthTexture, lAPosition).r;
        vec3 lNdcPosition = vec3(PositionA2N(lAPosition), lNdcDepth);
        vec3 lVPosition = PositionN2V(lNdcPosition, cameraInfo.info);
        vec3 h = vPosition - lVPosition;

        vec2 dAposition = clamp((gl_FragCoord.xy + vec2(0, 1)) * hbaoInfo.attachmentTexelSize, 0, 1);
        float dNdcDepth =  texture(depthTexture, dAposition).r;
        vec3 dNdcPosition = vec3(PositionA2N(dAposition), dNdcDepth);
        vec3 dVPosition = PositionN2V(dNdcPosition, cameraInfo.info);
        vec3 v = vPosition - dVPosition;

        faceVNormal = normalize(cross(h, v));
    }
    
    float noiseRadian = texture(noiseTexture, mod(gl_FragCoord.xy, hbaoInfo.noiseTextureWidth) / hbaoInfo.noiseTextureWidth).r * DOUBLE_PI;

    float occlusion = 0;
    for(int i = 0; i < hbaoInfo.directionCount; i++)
    {
        float directionRadian = 1.0 * i / hbaoInfo.directionCount * DOUBLE_PI + noiseRadian;
        vec3 direction = vec3(cos(directionRadian), sin(directionRadian), 0);
        vec3 vStep = direction * hbaoInfo.sampleRadius / hbaoInfo.stepCount;
        float sinTangent = -dot(direction, faceVNormal);
        float minSinHorizon = clamp(sin(hbaoInfo.sampleBiasAngle * PI / 180), 0, 1);

        float previousAo = 0;
        float wao = 0;
        for(int j = 0; j < hbaoInfo.stepCount; j++)
        {
            vec3 sampleVPosition = vPosition + vStep * (j + 0.5);
            vec4 samplePPosition = cameraInfo.info.projection * vec4(sampleVPosition, 1);
            if(samplePPosition.w == 0) continue;
            vec3 sampleNdcPosition = samplePPosition.xyz / samplePPosition.w;

            vec2 sampledAPosition = clamp(PositionN2A(sampleNdcPosition.xy), 0, 1);
            float sampledNdcDepth = texture(depthTexture, sampledAPosition).r;
            vec3 sampledNdcPosition = vec3(PositionA2N(sampledAPosition), sampledNdcDepth);
            vec3 sampledVPosition = PositionN2V(sampledNdcPosition, cameraInfo.info);

            vec3 vector = sampledVPosition - vPosition;

            float sinHorizon = vector.z / length(vector);
            float ao = clamp(sinHorizon - sinTangent - minSinHorizon, 0, 1);
            if(ao >= previousAo)
            {
                wao += clamp(1 - dot(vector, vector) / pow(hbaoInfo.sampleRadius, 2), 0, 1) * (ao - previousAo);
                previousAo = ao;
            }
        }
        occlusion += wao;
    }

    OcclusionTexture = occlusion / hbaoInfo.directionCount;
}
