#ifndef _BLUR_AMBIENT_OCCLUSION_GLSL_
#define _BLUR_AMBIENT_OCCLUSION_GLSL_

#define MAX_GAUSSION_KERNAL_RADIUS 4

const float GAUSSION_WEIGHT[MAX_GAUSSION_KERNAL_RADIUS] = {0.324, 0.232, 0.0855, 0.0205};

layout(set = 0, binding = 0) uniform BlurInfo
{
    vec2 attachmentSize;
    vec2 attachmentTexelSize;
    vec2 sampleOffset;
} blurInfo;

layout(set = 1, binding = 0) uniform sampler2D normalTexture;
layout(set = 2, binding = 0) uniform sampler2D occlusionTexture;

#define START_SET_INDEX 3

#endif ///#ifndef _BLUR_AMBIENT_OCCLUSION_GLSL_