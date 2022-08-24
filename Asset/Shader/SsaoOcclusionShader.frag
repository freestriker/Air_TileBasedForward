#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"
#include "Object.glsl"

#define SAMPLE_KERNAL_SIZE 64
#define PI (acos(-1.0))

layout(set = 0, binding = 0) uniform _CameraInfo
{
    CameraInfo info;
} cameraInfo;
layout(set = 1, binding = 0) uniform sampler2D noiseTexture;
layout(set = 2, binding = 0) uniform sampler2D depthTexture;
layout(set = 3, binding = 0) uniform sampler2D normalTexture;
layout(set = 4, binding = 0) uniform SizeInfo
{
    vec2 attachmentSize;
    vec2 noiseTextureSize;
    vec2 scale;
} sizeInfo;
layout(set = 5, binding = 0) uniform SampleKernal
{
    float radius;
    vec4 points[SAMPLE_KERNAL_SIZE];
} sampleKernal;

layout(location = 0) out float OcclusionAttachment;
layout(location = 1) out vec4 ColorAttachment;

void main()
{
    vec2 aPosition = gl_FragCoord.xy / sizeInfo.attachmentSize;

    float ndcDepth = texture(depthTexture, aPosition).r;
    vec3 ndcPosition = vec3(PositionA2N(aPosition), ndcDepth);
    vec3 vPosition = PositionN2V(ndcPosition, cameraInfo.info);
    vec3 vNormal = ParseFromColor(texture(normalTexture, aPosition).rgb);

    float noiseRadian = texture(noiseTexture, aPosition * sizeInfo.scale).r * 2 * PI;
    
    vec3 zAxis = vNormal;
    vec3 yAxis = normalize(cross(vec3(cos(noiseRadian), sin(noiseRadian), 0), zAxis));
    vec3 xAxis = normalize(cross(zAxis, yAxis));
    mat3 rotationMatrix = mat3(xAxis, yAxis, zAxis);

    float occlusion = 0;
    for(int i = 0; i < SAMPLE_KERNAL_SIZE; i++)
    {
        vec3 noisedSamplePoint = sampleKernal.points[i].xyz * sampleKernal.points[i].w * sampleKernal.radius;
        vec3 vSamplePoint = rotationMatrix * noisedSamplePoint + vPosition;
        vec4 pSamplePoint = cameraInfo.info.projection * vec4(vSamplePoint, 1);
        vec3 ndcSamplePoint = pSamplePoint.xyz / pSamplePoint.w;

        vec2 samplePointTexCoords = PositionN2A(ndcSamplePoint.xy);
        float samplePointNdcDepth = ndcSamplePoint.z;
        // float samplePointVDepth = DepthN2V(samplePointNdcDepth, cameraInfo.info);
        float visiableSamplePointNdcDepth = texture(depthTexture, samplePointTexCoords).r;
        // float visiableSamplePointVDepth = DepthN2V(visiableSamplePointNdcDepth, cameraInfo.info);

        occlusion += (visiableSamplePointNdcDepth < samplePointNdcDepth ? 1.0f : 0.0f);
        // occlusion += (samplePointVDepth < visiableSamplePointVDepth ? 1.0f : 0.0f);
    }

    OcclusionAttachment = occlusion / SAMPLE_KERNAL_SIZE;
}
