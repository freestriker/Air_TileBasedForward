#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"
#include "Object.glsl"

#define SAMPLE_KERNAL_SIZE 64

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
    int kernalSize;
    vec4 points[SAMPLE_KERNAL_SIZE];
} sampleKernal;

layout(location = 0) in vec2 outTexCoords;

layout(location = 0) out float OcclusionAttachment;

void main() 
{
    vec4 point = sampleKernal.points[SAMPLE_KERNAL_SIZE - 1];
    OcclusionAttachment = texture(noiseTexture, outTexCoords.xy * sizeInfo.scale).r * point.w;
}
