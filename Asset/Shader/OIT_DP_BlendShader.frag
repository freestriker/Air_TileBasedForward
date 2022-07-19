#version 450
#extension GL_GOOGLE_include_directive: enable

layout(set = 0, binding = 0) uniform sampler2D srcPeeledColorTexture_0;
layout(set = 1, binding = 0) uniform sampler2D srcPeeledColorTexture_1;
layout(set = 2, binding = 0) uniform sampler2D srcPeeledColorTexture_2;
layout(set = 3, binding = 0) uniform sampler2D srcPeeledColorTexture_3;

layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 colorAttachment;

void main() 
{
    vec4 dst = vec4(0, 0, 0, 1);
    vec4 src = vec4(0, 0, 0, 0);
    vec4 color = vec4(0, 0, 0, 0);

    src = texture(srcPeeledColorTexture_0, inTexCoords);
    color = vec4(dst.a * src.a * src.rgb + dst.rgb, (1 - src.a) * dst.a);
    dst = color;

    src = texture(srcPeeledColorTexture_1, inTexCoords);
    color = vec4(dst.a * src.a * src.rgb + dst.rgb, (1 - src.a) * dst.a);
    dst = color;

    src = texture(srcPeeledColorTexture_2, inTexCoords);
    color = vec4(dst.a * src.a * src.rgb + dst.rgb, (1 - src.a) * dst.a);
    dst = color;

    src = texture(srcPeeledColorTexture_3, inTexCoords);
    color = vec4(dst.a * src.a * src.rgb + dst.rgb, (1 - src.a) * dst.a);
    dst = color;

    colorAttachment = dst;
}
