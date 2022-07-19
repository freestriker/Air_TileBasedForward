#version 450
#extension GL_GOOGLE_include_directive: enable

#include "TBF_OIT_DepthPeelingLighting.glsl"

layout(location = 0) out vec4 colorAttachment;

void main() 
{
    DISCARD_PEELED_LAYER

    colorAttachment = vec4(1, 0, 0, 0.5);
}
