#version 450
#extension GL_GOOGLE_include_directive: enable

#include "BackgroundRendering.glsl"

layout(set = START_SET_INDEX + 0, binding = 0) uniform samplerCube backgroundTexture;

layout(location = 0) out vec4 ColorAttachment;

void main() 
{
    vec3 ndcPosition = vec3(PositionA2N(gl_FragCoord.xy * attachmentSizeInfo.texelSize), 1);
    vec3 wPosition = PositionN2W(ndcPosition, cameraInfo.info);
    vec3 observeDirection = CameraWObserveDirection(wPosition, cameraInfo.info);

    ColorAttachment = texture(backgroundTexture, observeDirection);
}
