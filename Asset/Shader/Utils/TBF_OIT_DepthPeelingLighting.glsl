#ifndef _TBF_OIT_DEPTH_PEELING_LIGHTING_GLSL_
#define _TBF_OIT_DEPTH_PEELING_LIGHTING_GLSL_

#include "TileBasedForwardLighting.glsl"

layout (set = START_SET_INDEX + 0, binding = 0, r32f) uniform readonly image2D depthPeelingThresholdImage;

#define DISCARD_PEELED_LAYER {float depth = gl_FragCoord.z; float depthPeelingThreshold = imageLoad(depthPeelingThresholdImage, ivec2(gl_FragCoord.xy)).r; if(depth <= depthPeelingThreshold) discard;}

#undef START_SET_INDEX
#define START_SET_INDEX 5

#endif ///#ifndef _TBF_OIT_DEPTH_PEELING_LIGHTING_GLSL_