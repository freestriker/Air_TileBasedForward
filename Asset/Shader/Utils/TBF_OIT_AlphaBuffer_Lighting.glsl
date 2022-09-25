#ifndef _TBF_OIT_ALPHA_BUFFER_LIGHTING_GLSL_
#define _TBF_OIT_ALPHA_BUFFER_LIGHTING_GLSL_

#include "TBForwardLighting.glsl"

#define LIST_SIZE_FACTOR 4

#ifdef FRAGMENT_STAGE
layout (early_fragment_tests) in;
layout (origin_upper_left) in vec4 gl_FragCoord;
#endif ///#ifdef FRAGMENT_STAGE

#define TBF_OIT_ALPHA_BUFFER_LIGHTING_DESCRIPTOR_COUNT (5 + TBFORWARD_LIGHTING_DESCRIPTOR_COUNT)

layout (set = START_SET_INDEX + 0, binding = 0, r32ui) uniform uimage2D headIndexImage;
layout (set = START_SET_INDEX + 1, binding = 0) writeonly buffer ColorList
{
    vec4[] colors;
}colorList;
layout (set = START_SET_INDEX + 2, binding = 0) writeonly buffer DepthList
{
    float[] depths;
}depthList;
layout (set = START_SET_INDEX + 3, binding = 0) writeonly buffer IndexList
{
    uint[] indexes;
}indexList;
layout (set = START_SET_INDEX + 4, binding = 0) buffer AtomicCounter
{
    uint counter;
}atomicCounter;

#undef START_SET_INDEX
#define START_SET_INDEX TBF_OIT_ALPHA_BUFFER_LIGHTING_DESCRIPTOR_COUNT

#define INVALID_BUFFER_INDEX 0
#define CURRENT_BUFFER_INDEX (atomicCounter.counter)
#define MAX_BUFFER_INDEX (LIST_SIZE_FACTOR * imageSize(headIndexImage).x * imageSize(headIndexImage).y - 1)

#ifdef FRAGMENT_STAGE
void AddColorToAlphaBuffer(in vec4 color)
{
    uint storeIndex = atomicAdd(CURRENT_BUFFER_INDEX, 1);
    if(storeIndex > MAX_BUFFER_INDEX) return;

    ivec2 pixelID = ivec2(gl_FragCoord.xy);
    float depth = gl_FragCoord.z;

    uint nextIndex = imageAtomicExchange(headIndexImage, pixelID, storeIndex);

    colorList.colors[storeIndex] = color;
    depthList.depths[storeIndex] = depth;
    indexList.indexes[storeIndex] = nextIndex;
}
#endif ///#ifdef FRAGMENT_STAGE

#endif ///#ifndef _TBF_OIT_ALPHA_BUFFER_LIGHTING_GLSL_