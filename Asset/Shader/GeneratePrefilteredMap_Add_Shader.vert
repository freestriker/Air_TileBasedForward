#version 450
#extension GL_GOOGLE_include_directive: enable

#include "BackgroundRendering.glsl"

layout(location = 0) in vec3 vertexPosition;

layout(location = 0) out vec3 worldPosition;

layout(push_constant) uniform PushConstantInfo
{
    uint stepCount;
    uint sliceCount;
    uint sliceIndex;
    uint resolution;
    uint faceIndex;
    float roughness;
    mat4 viewProjection;
} pushConstantInfo;

void main() 
{
    worldPosition = vertexPosition;
    gl_Position = pushConstantInfo.viewProjection * vec4(vertexPosition, 1.0);
}
