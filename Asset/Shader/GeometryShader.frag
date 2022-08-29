#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Object.glsl"

layout(location = 0) in vec3 inViewNormal;

layout(location = 0) out float DepthTexture;
layout(location = 1) out vec3 NormalTexture;

void main() 
{
    DepthTexture = gl_FragCoord.z;
    NormalTexture = ParseToColor(normalize(inViewNormal)).rgb;
}
