#version 450
#extension GL_GOOGLE_include_directive: enable

layout(set = 0, binding = 0) uniform sampler2D srcPeeledColorTexture;

layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 colorAttachment;

void main() 
{
    colorAttachment = texture(srcPeeledColorTexture, inTexCoords);
}
