#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"
#include "Object.glsl"

#define PI (acos(-1.0))
#define NOISE_COUNT 4096
#define SAMPLE_POINT_COUNT 32

layout(set = 0, binding = 0) uniform _CameraInfo
{
    CameraInfo info;
} cameraInfo;
layout(set = 1, binding = 0) uniform NoiseInfo
{
    vec4 noises[NOISE_COUNT / 4];
}noiseInfo;
layout(set = 2, binding = 0) uniform sampler2D depthTexture;
layout(set = 3, binding = 0) uniform sampler2D normalTexture;
layout(set = 4, binding = 0) uniform OcclusionTextureSizeInfo
{
    vec2 size;
    vec2 texelSize;
} occlusionTextureSizeInfo;
layout(set = 5, binding = 0) uniform SamplePointInfo
{
    vec4 samplePoints[SAMPLE_POINT_COUNT];
} samplePointInfo;

layout(location = 0) out float OcclusionTexture;

void main()
{
    vec2 aPosition = gl_FragCoord.xy * occlusionTextureSizeInfo.texelSize;
    
    vec3 ndcPosition = vec3(PositionA2N(aPosition), texture(depthTexture, aPosition).r);
    vec3 vPosition = PositionN2V(ndcPosition, cameraInfo.info);
    vec3 vNormal = ParseFromColor(texture(normalTexture, aPosition).rgb);

    ivec2 pixelIndexPosition = ivec2(gl_FragCoord.xy);
    int occlusionTextureWidth = int(occlusionTextureSizeInfo.size.x);
    int pixelIndex = pixelIndexPosition.y * occlusionTextureWidth + pixelIndexPosition.x;
    int noiseIndex = pixelIndex % NOISE_COUNT;
    float noiseRadian = noiseInfo.noises[noiseIndex / 4][noiseIndex % 4] * 2 * PI;
    
    vec3 zAxis = vNormal;
    vec3 yAxis = normalize(cross(zAxis, vec3(cos(noiseRadian), sin(noiseRadian), 0)));
    vec3 xAxis = normalize(cross(yAxis, zAxis));
    mat3 rotationMatrix = mat3(xAxis, yAxis, zAxis);

    float occlusion = 0;
    for(int i = 0; i < SAMPLE_POINT_COUNT; i++)
    {
        vec3 noisedSamplePoint = samplePointInfo.samplePoints[i].xyz * samplePointInfo.samplePoints[i].w;
        vec3 vSamplePoint = rotationMatrix * noisedSamplePoint + vPosition;
        vec4 pSamplePoint = cameraInfo.info.projection * vec4(vSamplePoint, 1);
        vec3 ndcSamplePoint = pSamplePoint.xyz / pSamplePoint.w;

        vec2 samplePointTexCoords = PositionN2A(ndcSamplePoint.xy);
        if(samplePointTexCoords.x != 0 && samplePointTexCoords.x != 1 && samplePointTexCoords.y != 0 && samplePointTexCoords.y != 1)
        {
            float samplePointNdcDepth = ndcSamplePoint.z;
            float visiableSamplePointNdcDepth = texture(depthTexture, samplePointTexCoords).r;
            vec3 visiableSamplePointVPosition = vec3(vSamplePoint.xy, DepthN2V(visiableSamplePointNdcDepth, cameraInfo.info));

            bool isInRadius = length(visiableSamplePointVPosition - vPosition) <= samplePointInfo.samplePoints[i].w;

            occlusion += ((visiableSamplePointNdcDepth < samplePointNdcDepth && isInRadius) ? 1.0f : 0.0f);
        }
    }

    OcclusionTexture = occlusion / SAMPLE_POINT_COUNT;
}