#ifndef _TBF_OIT_DEPTH_PEELING_LIGHTING_GLSL_
#define _TBF_OIT_DEPTH_PEELING_LIGHTING_GLSL_

#include "TileBasedForwardLighting.glsl"

layout (set = START_SET_INDEX + 0, binding = 0) uniform sampler2D depthPeelingThresholdTexture;
layout (set = START_SET_INDEX + 1, binding = 0, r32f) uniform readonly image2D depthImage;

#define DISCARD_PEELED_LAYER {float fragDepth = gl_FragCoord.z; float depth = imageLoad(depthImage, ivec2(gl_FragCoord.xy)).r; float thresholdDepth = texture(depthPeelingThresholdTexture, gl_FragCoord.xy / imageSize(depthImage)).r; if(depth <= fragDepth || fragDepth <= thresholdDepth) {discard;}}

#undef START_SET_INDEX
#define START_SET_INDEX 8

#endif ///#ifndef _TBF_OIT_DEPTH_PEELING_LIGHTING_GLSL_