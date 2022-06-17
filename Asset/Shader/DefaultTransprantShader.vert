#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Common.glsl"
#include "Camera.glsl"
#include "Light.glsl"

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoords;

layout(location = 0) out vec2 outTexCoords;

void main() {
    gl_Position = PositionObjectToProjection(vec4(vertexPosition, 1.0));

    outTexCoords = vertexTexCoords;
}
