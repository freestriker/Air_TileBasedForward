#version 450
#extension GL_GOOGLE_include_directive: enable

#include "BackgroundRendering.glsl"

layout(location = 0) in vec3 vertexPosition;
layout(location = 0) out vec3 worldPosition;

layout(push_constant) uniform GenerateVertexStageInfo
{
    uint stepCount;
    uint sliceCount;
    uint sliceIndex;
    mat4 viewProjection;
} generateInfo;

void main() 
{
    worldPosition = (vec4(vertexPosition, 1.0)).xyz;
    gl_Position = generateInfo.viewProjection * vec4(vertexPosition, 1.0);
}
