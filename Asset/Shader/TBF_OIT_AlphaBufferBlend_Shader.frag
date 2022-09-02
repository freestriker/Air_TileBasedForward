#version 450
#extension GL_GOOGLE_include_directive: enable

layout (origin_upper_left) in vec4 gl_FragCoord;

#define INVALID_BUFFER_INDEX 0

layout (set = 0, binding = 0, r32ui) uniform readonly uimage2D headIndexImage;
layout (set = 1, binding = 0) buffer ColorList
{
    vec4[] colors;
}colorList;
layout (set = 2, binding = 0) buffer DepthList
{
    float[] depths;
}depthList;
layout (set = 3, binding = 0) readonly buffer IndexList
{
    uint[] indexes;
}indexList;

layout(location = 0) out vec4 ColorAttachment;

void SwapPixel(in uint i, in uint j)
{
    float tDepth = depthList.depths[i];
    vec4 tColor = colorList.colors[i];

    depthList.depths[i] = depthList.depths[j];
    colorList.colors[i] = colorList.colors[j];

    depthList.depths[j] = tDepth;
    colorList.colors[j] = tColor;
}

void main() 
{
    uint pixelIndex = imageLoad(headIndexImage, ivec2(gl_FragCoord.xy)).r;

    if(pixelIndex == 0) discard;

    ///Bubble sort all pixels
    for(uint iIteratorIndex = pixelIndex; iIteratorIndex != INVALID_BUFFER_INDEX; iIteratorIndex = indexList.indexes[iIteratorIndex])
    {
        for(uint jIteratorIndex = pixelIndex; jIteratorIndex != INVALID_BUFFER_INDEX; jIteratorIndex = indexList.indexes[jIteratorIndex])
        {
            if(depthList.depths[jIteratorIndex] < depthList.depths[iIteratorIndex])
            {
                SwapPixel(iIteratorIndex, jIteratorIndex);
            }
        }
    }

    ///Under blend all pixels
    vec4 dst = vec4(0, 0, 0, 1);
    vec4 src = vec4(0, 0, 0, 0);
    vec4 color = vec4(0, 0, 0, 0);
    for(uint iteratorIndex = pixelIndex; iteratorIndex != INVALID_BUFFER_INDEX; iteratorIndex = indexList.indexes[iteratorIndex])
    {
        src = colorList.colors[iteratorIndex];
        color = vec4(dst.a * src.a * src.rgb + dst.rgb, (1 - src.a) * dst.a);
        dst = color;
    }

    ColorAttachment = dst;
}
