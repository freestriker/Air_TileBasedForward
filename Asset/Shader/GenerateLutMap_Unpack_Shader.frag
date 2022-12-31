#version 450
#extension GL_GOOGLE_include_directive: enable

layout(location = 0) out vec2 UnpackAttachment;

layout(push_constant) uniform UnpackInfo
{
    uint stepCount;
    uint resolution;
    uint placeholder0;
    uint placeholder1;
} unpackInfo;

layout(set = 0, binding = 0) uniform usampler2D packImage;

void main() 
{
    vec2 aPosition = gl_FragCoord.xy / vec2(unpackInfo.resolution, unpackInfo.resolution);

    uvec4 packed4 = texture(packImage, aPosition).rgba;
    uint packed = (packed4.r << 24) | (packed4.g << 16) | (packed4.b << 8) | (packed4.a);
    vec2 parameter = unpackHalf2x16(packed);

    UnpackAttachment = parameter;
}