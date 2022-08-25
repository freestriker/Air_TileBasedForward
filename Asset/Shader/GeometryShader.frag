#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Object.glsl"

layout(location = 0) in vec3 inViewNormal;

layout(location = 0) out vec4 NormalAttachment;

void main() 
{
    NormalAttachment = vec4(ParseToColor(normalize(inViewNormal)), 1);
}
