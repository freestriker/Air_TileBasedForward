#version 450
#extension GL_GOOGLE_include_directive: enable

layout(location = 0) out uvec4 PackAttachment;

layout(push_constant) uniform PackInfo
{
    uint stepCount;
    uint resolution;
    uint placeholder0;
    uint placeholder1;
} packInfo;

layout(set = 0, binding = 0) uniform sampler2D accumulationImage;

void main() 
{
    vec2 aPosition = gl_FragCoord.xy / vec2(packInfo.resolution, packInfo.resolution);

    vec2 parameter = texture(accumulationImage, aPosition).rg;

    uint packed = packHalf2x16(parameter);

    PackAttachment = uvec4((packed >> 24) & 255u, (packed >> 16) & 255u, (packed >> 8) & 255u, (packed) & 255u);
}