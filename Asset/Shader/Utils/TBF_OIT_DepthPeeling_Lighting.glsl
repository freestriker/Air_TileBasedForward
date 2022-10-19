#ifndef _TBF_OIT_DEPTH_PEELING_LIGHTING_GLSL_
#define _TBF_OIT_DEPTH_PEELING_LIGHTING_GLSL_

#include "TBForwardLighting.glsl"

#define TBF_OIT_DEPTH_PEELING_LIGHTING_DESCRIPTOR_COUNT (3 + TBFORWARD_LIGHTING_DESCRIPTOR_COUNT)

layout (set = START_SET_INDEX + 0, binding = 0) uniform sampler2D thresholdDepthTexture;
layout (set = START_SET_INDEX + 1, binding = 0) uniform sampler2D depthTexture;
layout (set = START_SET_INDEX + 2, binding = 0) uniform DepthPeelingInfo
{
    vec2 texelSize;
} depthPeelingInfo;

#define DISCARD_PEELED_LAYER { \
    float fragDepth = gl_FragCoord.z; \
    vec2 aPosition = gl_FragCoord.xy * depthPeelingInfo.texelSize; \
    float opaqueDepth = texture(depthTexture, aPosition).r; \
    float thresholdDepth = texture(thresholdDepthTexture, aPosition).r; \
    if(opaqueDepth <= fragDepth || fragDepth <= thresholdDepth) {discard;} \
} 

#ifdef START_SET_INDEX
#undef START_SET_INDEX
#endif ///#ifdef START_SET_INDEX
#define START_SET_INDEX TBF_OIT_DEPTH_PEELING_LIGHTING_DESCRIPTOR_COUNT

#endif ///#ifndef _TBF_OIT_DEPTH_PEELING_LIGHTING_GLSL_