#ifndef _TBF_OIT_ALPHA_LINKED_LIST_LIGHTING_GLSL_
#define _TBF_OIT_ALPHA_LINKED_LIST_LIGHTING_GLSL_

#include "TileBasedForwardLighting.glsl"

#define ALPHA_LINKED_LIST_SIZE_FACTOR 4

#ifdef FRAGMENT_STAGE
layout (early_fragment_tests) in;
layout (origin_upper_left) in vec4 gl_FragCoord;
#endif ///#ifdef FRAGMENT_STAGE

layout (set = START_SET_INDEX + 0, binding = 0, r32ui) uniform uimage2D linkedListHeadImage;
layout (set = START_SET_INDEX + 1, binding = 0) writeonly buffer PixelColors
{
    vec4[] colors;
}pixelColors;
struct PixelInfo
{
    float depth;
    uint previousPixelIndex;
};
layout (set = START_SET_INDEX + 2, binding = 0) writeonly buffer PixelInfos
{
    PixelInfo[] infos;
}pixelInfos;
layout (set = START_SET_INDEX + 3, binding = 0) buffer PixelAtomicCounter
{
    uint currentIndex;
}pixelAtomicCounter;

#undef START_SET_INDEX
#define START_SET_INDEX 8

#define PIXEL_CURRENT_INDEX (pixelAtomicCounter.currentIndex)
#define PIXEL_MAX_INDEX (ALPHA_LINKED_LIST_SIZE_FACTOR * imageSize(linkedListHeadImage).x * imageSize(linkedListHeadImage).y)

#ifdef FRAGMENT_STAGE
void AddColorToAlphaLinkedList(vec4 color)
{
    uint pixelMaxIndex = PIXEL_MAX_INDEX;
    if(PIXEL_CURRENT_INDEX >= pixelMaxIndex) return;

    uint storeIndex = atomicAdd(PIXEL_CURRENT_INDEX, 1);

    if(storeIndex > pixelMaxIndex) return;

    ivec2 pixelID = ivec2(gl_FragCoord.xy);
    float pixelDepth = gl_FragCoord.z;
    uint previousIndex = imageAtomicExchange(linkedListHeadImage, pixelID, storeIndex);

    pixelColors.colors[storeIndex] = color;
    pixelInfos.infos[storeIndex].depth = pixelDepth;
    pixelInfos.infos[storeIndex].previousPixelIndex = previousIndex;
}
#endif ///#ifdef FRAGMENT_STAGE

#endif ///#ifndef _TBF_OIT_ALPHA_LINKED_LIST_LIGHTING_GLSL_