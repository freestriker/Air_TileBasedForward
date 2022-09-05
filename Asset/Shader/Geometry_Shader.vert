#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"
#include "Object.glsl"

layout(set = 0, binding = 0) uniform _CameraInfo
{
    CameraInfo info;
} cameraInfo;

layout(set = 1, binding = 0) uniform MeshObjectInfo
{
    ObjectInfo info;
} meshObjectInfo;

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

layout(location = 0) out vec3 outViewNormal;

void main() 
{
    gl_Position = PositionO2P(vertexPosition, meshObjectInfo.info, cameraInfo.info);

    outViewNormal = DirectionTransition(vertexNormal, mat3(cameraInfo.info.view * meshObjectInfo.info.model));
}
