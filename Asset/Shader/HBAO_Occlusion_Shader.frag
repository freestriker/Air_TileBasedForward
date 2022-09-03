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
    float stepCount;
    float directionCount;
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
    //Recontruction faceNormal
    // {
    //     float surroundingNdcDepths[4];
    //     vec3 surroundingVPositions[4];
    //     vec3 horizontal;
    //     vec3 vertical;

    //     surroundingNdcDepths[0] = texture(depthTexture, clamp(aPosition + vec2(hbaoInfo.attachmentTexelSize.x, 0), 0, 1)).r;
    //     surroundingNdcDepths[1] = texture(depthTexture, clamp(aPosition - vec2(hbaoInfo.attachmentTexelSize.x, 0), 0, 1)).r;
    //     surroundingNdcDepths[2] = texture(depthTexture, clamp(aPosition + 2 * vec2(hbaoInfo.attachmentTexelSize.x, 0), 0, 1)).r;
    //     surroundingNdcDepths[3] = texture(depthTexture, clamp(aPosition - 2 * vec2(hbaoInfo.attachmentTexelSize.x, 0), 0, 1)).r;

    //     surroundingVPositions[0] = PositionN2V(vec3(clamp(aPosition + vec2(hbaoInfo.attachmentTexelSize.x, 0), 0, 1), surroundingNdcDepths[0]), cameraInfo.info);
    //     surroundingVPositions[1] = PositionN2V(vec3(clamp(aPosition - vec2(hbaoInfo.attachmentTexelSize.x, 0), 0, 1), surroundingNdcDepths[1]), cameraInfo.info);
    //     surroundingVPositions[2] = PositionN2V(vec3(clamp(aPosition + 2 * vec2(hbaoInfo.attachmentTexelSize.x, 0), 0, 1), surroundingNdcDepths[2]), cameraInfo.info);
    //     surroundingVPositions[3] = PositionN2V(vec3(clamp(aPosition - 2 * vec2(hbaoInfo.attachmentTexelSize.x, 0), 0, 1), surroundingNdcDepths[3]), cameraInfo.info);

    //     float rightNewDepth = (surroundingVPositions[0].z - surroundingVPositions[2].z) / (surroundingVPositions[0].x - surroundingVPositions[2].x) * (vPosition.x - surroundingVPositions[0].x);
    //     float leftNewDepth = (surroundingVPositions[1].z - surroundingVPositions[3].z) / (surroundingVPositions[1].x - surroundingVPositions[3].x) * (vPosition.x - surroundingVPositions[1].x);
    //     if(abs(rightNewDepth - vDepth) < abs(leftNewDepth - vDepth))
    //     {
    //         horizontal = surroundingVPositions[0] - vPosition;
    //     }
    //     else
    //     {
    //         horizontal = vPosition - surroundingVPositions[1];
    //     }

    //     surroundingNdcDepths[0] = texture(depthTexture, clamp(aPosition + vec2(0, hbaoInfo.attachmentTexelSize.y), 0, 1)).r;
    //     surroundingNdcDepths[1] = texture(depthTexture, clamp(aPosition - vec2(0, hbaoInfo.attachmentTexelSize.y), 0, 1)).r;
    //     surroundingNdcDepths[2] = texture(depthTexture, clamp(aPosition + 2 * vec2(0, hbaoInfo.attachmentTexelSize.y), 0, 1)).r;
    //     surroundingNdcDepths[3] = texture(depthTexture, clamp(aPosition - 2 * vec2(0, hbaoInfo.attachmentTexelSize.y), 0, 1)).r;

    //     surroundingVPositions[0] = PositionN2V(vec3(clamp(aPosition + vec2(0, hbaoInfo.attachmentTexelSize.y), 0, 1), surroundingNdcDepths[0]), cameraInfo.info);
    //     surroundingVPositions[1] = PositionN2V(vec3(clamp(aPosition - vec2(0, hbaoInfo.attachmentTexelSize.y), 0, 1), surroundingNdcDepths[1]), cameraInfo.info);
    //     surroundingVPositions[2] = PositionN2V(vec3(clamp(aPosition + 2 * vec2(0, hbaoInfo.attachmentTexelSize.y), 0, 1), surroundingNdcDepths[2]), cameraInfo.info);
    //     surroundingVPositions[3] = PositionN2V(vec3(clamp(aPosition - 2 * vec2(0, hbaoInfo.attachmentTexelSize.y), 0, 1), surroundingNdcDepths[3]), cameraInfo.info);

    //     float downNewDepth = (surroundingVPositions[0].z - surroundingVPositions[2].z) / (surroundingVPositions[0].y - surroundingVPositions[2].y) * (vPosition.y - surroundingVPositions[0].y);
    //     float upNewDepth = (surroundingVPositions[1].z - surroundingVPositions[3].z) / (surroundingVPositions[1].y - surroundingVPositions[3].y) * (vPosition.y - surroundingVPositions[1].y);
    //     if(abs(downNewDepth - vDepth) < abs(upNewDepth - vDepth))
    //     {
    //         vertical = vPosition - surroundingVPositions[0];
    //     }
    //     else
    //     {
    //         vertical = surroundingVPositions[1] - vPosition;
    //     }
    //     faceVNormal = normalize(cross(horizontal, vertical));
    // }
    {
        vec2 lAPosition = clamp(aPosition - vec2(hbaoInfo.attachmentTexelSize.x, 0), 0, 1);
        float lNdcDepth =  texture(depthTexture, lAPosition).r;
        vec3 lNdcPosition = vec3(PositionA2N(lAPosition), lNdcDepth);
        vec3 lVPosition = PositionN2V(lNdcPosition, cameraInfo.info);
        vec3 h = vPosition - lVPosition;

        vec2 dAposition = clamp(aPosition + vec2(0, hbaoInfo.attachmentTexelSize.y), 0, 1);
        float dNdcDepth =  texture(depthTexture, dAposition).r;
        vec3 dNdcPosition = vec3(PositionA2N(dAposition), dNdcDepth);
        vec3 dVPosition = PositionN2V(dNdcPosition, cameraInfo.info);
        vec3 v = vPosition - dVPosition;

        faceVNormal = normalize(cross(h, v));
    }

    float noiseRadian = texture(noiseTexture, mod(gl_FragCoord.xy, hbaoInfo.noiseTextureWidth) / hbaoInfo.noiseTextureWidth).r;

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
            vec3 sampleVPosition = vPosition + vStep * (j + 1);
            vec4 samplePPosition = cameraInfo.info.projection * vec4(sampleVPosition, 1);
            if(samplePPosition.w == 0) continue;
            vec3 sampleNdcPosition = samplePPosition.xyz / samplePPosition.w;

            vec2 sampledAPosition = clamp(PositionN2A(sampleNdcPosition.xy), 0, 1);
            float sampledNdcDepth = texture(depthTexture, sampledAPosition).r;
            vec3 sampledNdcPosition = vec3(PositionA2N(sampledAPosition), sampledNdcDepth);
            vec3 sampledVPosition = PositionN2V(sampledNdcPosition, cameraInfo.info);

            vec3 vector = sampledVPosition - vPosition;

            float sinHorizon = clamp(vector.z / length(vector), 0, 1);
            sinHorizon = sinHorizon < minSinHorizon ? 0 : sinHorizon;
            float ao = clamp(sinHorizon - sinTangent, 0, 1);
            if(ao >= previousAo)
            {
                wao += (1 - dot(vector, vector) / pow(hbaoInfo.sampleRadius, 2)) * (ao - previousAo);
                previousAo = ao;
            }
        }

        occlusion += wao;
    }

    OcclusionTexture = occlusion / hbaoInfo.directionCount;
}
