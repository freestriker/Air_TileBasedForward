#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Common.glsl"
#include "Camera.glsl"
#include "Light.glsl"

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

layout(location = 0) out vec3 outWorldPosition;
layout(location = 1) out vec3 outWorldNormal;

void main() 
{
    vec3 worldNormal = DirectionObjectToWorld(vertexNormal);
    vec3 worldPosition = PositionObjectToWorld(vec4(vertexPosition, 1.0)).xyz;

    outWorldPosition = worldPosition;
    outWorldNormal = worldNormal;
    gl_Position = PositionObjectToProjection(vec4(vertexPosition, 1.0));
}
