#version 450
#extension GL_GOOGLE_include_directive: enable

layout(set = 0, binding = 0) uniform sampler2D srcPeeledColorTexture;
layout(set = 1, binding = 0, r32f) uniform readonly image2D srcPeeledDepthImage;
layout(set = 2, binding = 0, r32f) uniform readonly image2D depthImage;

layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 colorAttachment;

void main() 
{
    ivec2 pixelOffset = ivec2(gl_FragCoord.xy);
    float pixelNdcDepth = imageLoad(depthImage, pixelOffset).r;
    float peeledNdcDepth = imageLoad(srcPeeledDepthImage, pixelOffset).r;
    if(peeledNdcDepth < pixelNdcDepth)
    {
        colorAttachment = texture(srcPeeledColorTexture, inTexCoords);
    }
    else
    {
        discard;
    }
}
