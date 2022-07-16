#version 450
#extension GL_GOOGLE_include_directive: enable

#include "TBF_OIT_DepthPeelingLighting.glsl"

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

layout(location = 0) out vec3 outWorldPosition;
layout(location = 1) out vec3 outWorldNormal;

void main() 
{
    gl_Position = PositionO2P(vertexPosition, meshObjectInfo.info, cameraInfo.info);
    vec3 worldNormal = DirectionO2W(vertexNormal, meshObjectInfo.info);
    vec3 worldPosition = PositionO2W(vertexPosition, meshObjectInfo.info);

    outWorldPosition = worldPosition;
    outWorldNormal = worldNormal;
}
