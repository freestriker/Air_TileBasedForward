#ifndef _TBF_OIT_DEPTH_PEELING_LIGHTING_GLSL_
#define _TBF_OIT_DEPTH_PEELING_LIGHTING_GLSL_

#include "TBForwardLighting.glsl"

layout (set = START_SET_INDEX + 0, binding = 0) uniform sampler2D thresholdDepthTexture;
layout (set = START_SET_INDEX + 1, binding = 0) uniform sampler2D depthTexture;
layout (set = START_SET_INDEX + 2, binding = 0) uniform AttachmentSizeInfo
{
    vec2 size;
    vec2 texelSize;
} attachmentSizeInfo;

#define DISCARD_PEELED_LAYER {float fragDepth = gl_FragCoord.z; vec2 aPosition = gl_FragCoord.xy * attachmentSizeInfo.texelSize; float depth = texture(depthTexture, aPosition).r; float thresholdDepth = texture(thresholdDepthTexture, aPosition).r; if(depth <= fragDepth || fragDepth <= thresholdDepth) {discard;}}

#undef START_SET_INDEX
#define START_SET_INDEX 9

#endif ///#ifndef _TBF_OIT_DEPTH_PEELING_LIGHTING_GLSL_