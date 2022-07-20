#version 450
#extension GL_GOOGLE_include_directive: enable

layout (origin_upper_left) in vec4 gl_FragCoord;

layout (set = 0, binding = 0, r32ui) uniform readonly uimage2D linkedListHeadImage;
layout (set = 1, binding = 0) buffer PixelColors
{
    vec4[] colors;
}pixelColors;
struct PixelInfo
{
    float depth;
    uint previousPixelIndex;
};
layout (set = 2, binding = 0) buffer PixelInfos
{
    PixelInfo[] infos;
}pixelInfos;

layout(location = 0) out vec4 colorAttachment;

void SwapPixel(uint i, uint j)
{
    if (i == j) return;

    float tDepth = pixelInfos.infos[i].depth;
    vec4 tColor = pixelColors.colors[i];

    pixelInfos.infos[i].depth = pixelInfos.infos[j].depth;
    pixelColors.colors[i] = pixelColors.colors[j];

    pixelInfos.infos[j].depth = tDepth;
    pixelColors.colors[j] = tColor;
}

void main() 
{
    uint pixelIndex = imageLoad(linkedListHeadImage, ivec2(gl_FragCoord.xy)).r;

    if(pixelIndex == 0) discard;

    ///Bubble sort all pixels
    for(uint iIteratorIndex = pixelIndex; iIteratorIndex > 0; iIteratorIndex = pixelInfos.infos[iIteratorIndex].previousPixelIndex)
    {
        for(uint jIteratorIndex = pixelIndex; jIteratorIndex > 0; jIteratorIndex = pixelInfos.infos[jIteratorIndex].previousPixelIndex)
        {
            if(pixelInfos.infos[jIteratorIndex].depth < pixelInfos.infos[iIteratorIndex].depth)
            {
                SwapPixel(iIteratorIndex, jIteratorIndex);
            }
        }
    }

    ///Under blend all pixels
    vec4 dst = vec4(0, 0, 0, 1);
    vec4 src = vec4(0, 0, 0, 0);
    vec4 color = vec4(0, 0, 0, 0);
    for(uint iteratorIndex = pixelIndex; iteratorIndex > 0; iteratorIndex = pixelInfos.infos[iteratorIndex].previousPixelIndex)
    {
        src = pixelColors.colors[iteratorIndex];
        color = vec4(dst.a * src.a * src.rgb + dst.rgb, (1 - src.a) * dst.a);
        dst = color;
    }

    colorAttachment = dst;
}
