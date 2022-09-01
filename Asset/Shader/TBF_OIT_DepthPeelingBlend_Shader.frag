#version 450
#extension GL_GOOGLE_include_directive: enable

layout(set = 0, binding = 0) uniform sampler2D colorTexture_0;
layout(set = 1, binding = 0) uniform sampler2D colorTexture_1;
layout(set = 2, binding = 0) uniform sampler2D colorTexture_2;
layout(set = 3, binding = 0) uniform sampler2D colorTexture_3;
layout (set = 4, binding = 0) uniform AttachmentSizeInfo
{
    vec2 size;
    vec2 texelSize;
} attachmentSizeInfo;

layout(location = 0) out vec4 ColorAttachment;

void main() 
{
    vec4 dst = vec4(0, 0, 0, 1);
    vec4 src = vec4(0, 0, 0, 0);
    vec4 color = vec4(0, 0, 0, 0);

    vec2 aPosition = gl_FragCoord.xy * attachmentSizeInfo.texelSize;

    src = texture(colorTexture_0, aPosition);
    color = vec4(dst.a * src.a * src.rgb + dst.rgb, (1 - src.a) * dst.a);
    dst = color;

    src = texture(colorTexture_1, aPosition);
    color = vec4(dst.a * src.a * src.rgb + dst.rgb, (1 - src.a) * dst.a);
    dst = color;

    src = texture(colorTexture_2, aPosition);
    color = vec4(dst.a * src.a * src.rgb + dst.rgb, (1 - src.a) * dst.a);
    dst = color;

    src = texture(colorTexture_3, aPosition);
    color = vec4(dst.a * src.a * src.rgb + dst.rgb, (1 - src.a) * dst.a);
    dst = color;

    ColorAttachment = dst;
}
