#version 450
#extension GL_GOOGLE_include_directive: enable

layout(set = 0, binding = 0) uniform sampler2D srcColorTexture;

layout(location = 0) in vec2 outTexCoords;

layout(location = 0) out vec4 colorAttachment;

void main() 
{
    colorAttachment = vec4(texture(srcColorTexture, outTexCoords.xy).xyz, 1);
}
