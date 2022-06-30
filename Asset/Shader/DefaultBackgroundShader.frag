#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Common.glsl"
#include "Camera.glsl"
#include "Light.glsl"

layout(set = START_SET + 0, binding = 0) uniform sampler2D depthTexture;
layout(set = START_SET + 1, binding = 0) uniform samplerCube backgroundTexture;

layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 colorAttachment;

void main() {
    if(texture(depthTexture, inTexCoords).x == 1.0)
    {
        vec3 worldPosition = PositionScreenToNearFlatWorld(inTexCoords);
        vec3 worldView = CameraWorldViewToPosition(worldPosition);
        colorAttachment = vec4(texture(backgroundTexture, worldView).xyz, 1);
    }
    else
    {
        discard;
    }
}
