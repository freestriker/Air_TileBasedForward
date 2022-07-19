#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"

layout(origin_upper_left) in vec4 gl_FragCoord;

layout(set = 0, binding = 0) uniform _CameraInfo
{
    CameraInfo info;
} cameraInfo;

// layout(set = 1, binding = 0, r32f) uniform readonly image2D depthImage;
// layout(set = 2, binding = 0) uniform samplerCube backgroundTexture;
layout(set = 1, binding = 0) uniform samplerCube backgroundTexture;

layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 colorAttachment;

void main() 
{
    // ivec2 pixelOffset = ivec2(gl_FragCoord.xy);
    // float pixelNdcDepth = imageLoad(depthImage, pixelOffset).r;

    // if(pixelNdcDepth == 1.0)
    // {
    //     vec3 worldPosition = PositionS2NFW(inTexCoords, cameraInfo.info);
    //     vec3 worldView = CameraWObserveDirection(worldPosition, cameraInfo.info);
    //     colorAttachment = texture(backgroundTexture, worldView);
    // }
    // else
    // {
    //     discard;
    // }
    vec3 worldPosition = PositionS2NFW(inTexCoords, cameraInfo.info);
    vec3 worldView = CameraWObserveDirection(worldPosition, cameraInfo.info);
    colorAttachment = texture(backgroundTexture, worldView);

}
