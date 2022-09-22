#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Object.glsl"
#include "Camera.glsl"

layout(set = 0, binding = 0) uniform _CameraInfo
{
    CameraInfo info;
} cameraInfo;
layout(set = 1, binding = 0) uniform VisualizationInfo
{
    vec2 size;
    vec2 texelSize;
} visualizationInfo;
layout(set = 2, binding = 0) uniform sampler2D depthTexture;

#define CASCADE_EVSM_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX 3
#include "CascadeEVSM_ShadowReceiver.glsl"

layout(location = 0) out vec4 ColorAttachment;

void main()
{
    vec2 aPosition = gl_FragCoord.xy * visualizationInfo.texelSize;
 
    vec3 ndcPosition = vec3(PositionA2N(aPosition), texture(depthTexture, aPosition).r);
    vec3 vPosition = PositionN2V(ndcPosition, cameraInfo.info);

    float shadowIntensity = GetShadowIntensity(vPosition);

    ColorAttachment = vec4(0, 0, 0, clamp(1.0f - shadowIntensity, 0, 1));
}
