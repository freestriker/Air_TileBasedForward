#version 450
#extension GL_GOOGLE_include_directive: enable

#define DEPTH_PEELING_STEP_COUNT 4

layout(set = 0, binding = 0) uniform sampler2D srcPeeledColorTexture_0;
layout(set = 1, binding = 0) uniform sampler2D srcPeeledColorTexture_1;
layout(set = 2, binding = 0) uniform sampler2D srcPeeledColorTexture_2;
layout(set = 3, binding = 0) uniform sampler2D srcPeeledColorTexture_3;

layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 colorAttachment;

void main() 
{
    vec4 srcColor = vec4(0, 0, 0, 0);
    vec4 dstColor = vec4(0, 0, 0, 0);
    vec4 color = texture(srcPeeledColorTexture_0, inTexCoords);

    dstColor = color;
    srcColor = texture(srcPeeledColorTexture_1, inTexCoords);
    color = vec4(dstColor.a * dstColor.rgb + (1 - dstColor.a) * srcColor.a * srcColor.rgb, (1 - dstColor.a) * srcColor.a + dstColor.a);

    dstColor = color;
    srcColor = texture(srcPeeledColorTexture_2, inTexCoords);
    color = vec4(dstColor.a * dstColor.rgb + (1 - dstColor.a) * srcColor.a * srcColor.rgb, (1 - dstColor.a) * srcColor.a + dstColor.a);

    dstColor = color;
    srcColor = texture(srcPeeledColorTexture_3, inTexCoords);
    color = vec4(dstColor.a * dstColor.rgb + (1 - dstColor.a) * srcColor.a * srcColor.rgb, (1 - dstColor.a) * srcColor.a + dstColor.a);
   
    colorAttachment = color;
}
