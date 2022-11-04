#version 450
#extension GL_GOOGLE_include_directive: enable

#include "BackgroundRendering.glsl"

layout(location = 0) in vec3 vertexPosition;
layout(location = 0) out vec3 worldPosition;

layout(push_constant) uniform SamplePointInfo
{
    mat4 view;
    mat4 viewProjection;
} splitInfo;

void main() 
{
    worldPosition = (splitInfo.view * vec4(vertexPosition, 1.0)).xyz;
    gl_Position = splitInfo.viewProjection * vec4(vertexPosition, 1.0);
}
