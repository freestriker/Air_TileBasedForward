#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"
#include "Object.glsl"

#define PI (acos(-1.0))
#define DOUBLE_PI (2 * acos(-1.0))
#define HALF_PI (0.5 * acos(-1.0))

layout(set = 0, binding = 0) uniform _CameraInfo
{
    CameraInfo info;
} cameraInfo;
layout(set = 1, binding = 0) uniform sampler2D noiseTexture;
layout(set = 2, binding = 0) uniform sampler2D depthTexture;
layout(set = 3, binding = 0) uniform GtaoInfo
{
    vec2 attachmentSize;
    vec2 attachmentTexelSize;
    float sampleRadius;
    float sampleBiasAngle;
    int stepCount;
    int sliceCount;
    int noiseTextureWidth;
} gtaoInfo;

layout(location = 0) out float OcclusionTexture;

void main()
{
    vec2 aPosition = gl_FragCoord.xy * gtaoInfo.attachmentTexelSize;
    float ndcDepth =  texture(depthTexture, aPosition).r;
    vec3 ndcPosition = vec3(PositionA2N(aPosition), ndcDepth);
    vec3 vPosition = PositionN2V(ndcPosition, cameraInfo.info);
    float vDepth = vPosition.z;

    vec3 faceVNormal;
    {
        vec2 lAPosition = clamp((gl_FragCoord.xy + vec2(-1, 0)) * gtaoInfo.attachmentTexelSize, 0, 1);
        float lNdcDepth =  texture(depthTexture, lAPosition).r;
        vec3 lNdcPosition = vec3(PositionA2N(lAPosition), lNdcDepth);
        vec3 lVPosition = PositionN2V(lNdcPosition, cameraInfo.info);
        vec3 h = vPosition - lVPosition;

        vec2 dAposition = clamp((gl_FragCoord.xy + vec2(0, 1)) * gtaoInfo.attachmentTexelSize, 0, 1);
        float dNdcDepth =  texture(depthTexture, dAposition).r;
        vec3 dNdcPosition = vec3(PositionA2N(dAposition), dNdcDepth);
        vec3 dVPosition = PositionN2V(dNdcPosition, cameraInfo.info);
        vec3 v = vPosition - dVPosition;

        faceVNormal = normalize(cross(h, v));
    }
    
    float noiseRadian = texture(noiseTexture, mod(gl_FragCoord.xy, gtaoInfo.noiseTextureWidth) / gtaoInfo.noiseTextureWidth).r * DOUBLE_PI;

    float occlusion = 0;
    for(int i = 0; i < gtaoInfo.sliceCount; i++)
    {
        float directionRadian = 1.0 * i / gtaoInfo.sliceCount / 2 * DOUBLE_PI + noiseRadian;
        vec3 direction = vec3(cos(directionRadian), sin(directionRadian), 0);
        vec3 vStep = direction * gtaoInfo.sampleRadius / gtaoInfo.stepCount;

        vec3 sliceTangentDirection = normalize(cross(direction, vec3(0, 0, 1)));
        vec3 slicePlainNormal = faceVNormal - dot(faceVNormal, sliceTangentDirection) * sliceTangentDirection;
        vec3 slicePlainNormalDirection = normalize(slicePlainNormal);

        float theta = acos(dot(slicePlainNormalDirection, vec3(0, 0, 1)));
        theta = dot(slicePlainNormalDirection, direction) < 0 ? theta : -theta;

        float maxRightHorizonRadian = -HALF_PI;
        ///Right
        for(int j = 0; j < gtaoInfo.stepCount; j++)
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

            float horizonRadian = asin(vector.z / length(vector)) - HALF_PI;
            maxRightHorizonRadian = maxRightHorizonRadian < horizonRadian ? horizonRadian : maxRightHorizonRadian;
        }
        float rh = clamp(maxRightHorizonRadian, theta - HALF_PI + gtaoInfo.sampleBiasAngle * PI / 180, 0);
        float rao = -cos(2 * rh - theta) + cos(theta) + 2 * rh * sin(theta);

        float minLeftHorizonRadian = HALF_PI;
        ///Left
        for(int j = 0; j < gtaoInfo.stepCount; j++)
        {
            vec3 sampleVPosition = vPosition - vStep * (j + 1);
            vec4 samplePPosition = cameraInfo.info.projection * vec4(sampleVPosition, 1);
            if(samplePPosition.w == 0) continue;
            vec3 sampleNdcPosition = samplePPosition.xyz / samplePPosition.w;

            vec2 sampledAPosition = clamp(PositionN2A(sampleNdcPosition.xy), 0, 1);
            float sampledNdcDepth = texture(depthTexture, sampledAPosition).r;
            vec3 sampledNdcPosition = vec3(PositionA2N(sampledAPosition), sampledNdcDepth);
            vec3 sampledVPosition = PositionN2V(sampledNdcPosition, cameraInfo.info);

            vec3 vector = sampledVPosition - vPosition;

            float horizonRadian = HALF_PI - asin(vector.z / length(vector));
            minLeftHorizonRadian = minLeftHorizonRadian > horizonRadian ? horizonRadian : minLeftHorizonRadian;
        }
        float lh = clamp(minLeftHorizonRadian, 0, theta + HALF_PI - gtaoInfo.sampleBiasAngle * PI / 180);
        float lao = -cos(2 * lh - theta) + cos(theta) + 2 * lh * sin(theta);

        float ao = length(slicePlainNormal) * 0.25 * (rao + lao);

        occlusion += ao;
    }

    OcclusionTexture = clamp(occlusion / gtaoInfo.sliceCount, 0, 1);
}
