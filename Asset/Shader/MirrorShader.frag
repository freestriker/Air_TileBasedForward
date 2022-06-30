#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Common.glsl"
#include "Camera.glsl"
#include "Light.glsl"

layout(location = 0) in vec3 inWorldPosition;
layout(location = 1) in vec3 inWorldNormal;

layout(location = 0) out vec4 colorAttachment;

void main() 
{
    vec3 worldNormal = normalize(inWorldNormal);
    vec3 worldView = CameraWorldViewToPosition(inWorldPosition);

    vec3 environment = vec3(0, 0, 0);
    environment += SkyBoxLighting(normalize(reflect(worldView, worldNormal)));

    colorAttachment = vec4(environment * 1.8, 1);
}
