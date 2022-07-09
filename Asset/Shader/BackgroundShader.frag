#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"

layout(set = 0, binding = 0) uniform _CameraInfo
{
    CameraInfo info;
} cameraInfo;
layout(set = 1, binding = 0) uniform sampler2D depthTexture;
layout(set = 2, binding = 0) uniform samplerCube backgroundTexture;

layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 colorAttachment;

void main() 
{
    if(texture(depthTexture, inTexCoords).x == 1.0)
    {
        vec3 worldPosition = PositionS2NFW(inTexCoords, cameraInfo.info);
        vec3 worldView = CameraWObserveDirection(worldPosition, cameraInfo.info);
        colorAttachment = vec4(texture(backgroundTexture, worldView).xyz, 1);
    }
    else
    {
        discard;
    }
}
