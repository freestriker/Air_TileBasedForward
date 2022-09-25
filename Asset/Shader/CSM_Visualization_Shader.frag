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
layout(set = 3, binding = 0) uniform sampler2D normalTexture;

#define CSM_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX 4
#include "CSM_ShadowReceiver.glsl"

layout(location = 0) out vec4 ColorAttachment;

void main()
{
    vec2 aPosition = gl_FragCoord.xy * visualizationInfo.texelSize;
 
    vec3 ndcPosition = vec3(PositionA2N(aPosition), texture(depthTexture, aPosition).r);
    vec3 vPosition = PositionN2V(ndcPosition, cameraInfo.info);
    vec3 vNormal = normalize(ParseFromColor(texture(normalTexture, aPosition).rgb));

    float shadowIntensity = GetShadowIntensityWithVNormal(vPosition, vNormal);

    ColorAttachment = vec4(0, 0, 0, clamp(1.0f - shadowIntensity, 0, 1));
}
