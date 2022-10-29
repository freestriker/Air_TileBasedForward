#version 450
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_shader_atomic_float: enable

#include "TBForwardLighting.glsl"

layout(location = 0) in vec2 inTexCoords;
layout(location = 1) in vec3 inWorldPosition;
layout(location = 2) in vec3 inWorldNormal;

layout(location = 0) out vec4 ColorAttachment;

layout(set = START_SET_INDEX + 0, binding = 0) uniform samplerCube backgroundTexture;

void main() 
{
    vec3 wNormal = normalize(inWorldNormal);
    vec3 wView = CameraWObserveDirection(inWorldPosition, cameraInfo.info);
    vec3 rmo = vec3(0.5, 1, 1);
    vec3 albedo = 4 * texture(backgroundTexture, normalize(reflect(wView, wNormal))).rgb;

    vec3 radiance = vec3(0, 0, 0);

    radiance += PbrLighting(lightInfos.mainLightInfo, inWorldPosition, wView, wNormal, albedo, rmo.x, rmo.y);
    for(int i = 0; i < opaqueLightIndexList.count; i++)
    {
        radiance += PbrLighting(lightInfos.ortherLightInfos[opaqueLightIndexList.indexes[i]], inWorldPosition, wView, wNormal, albedo, rmo.x, rmo.y);
    }

    float shadowIntensity = GetShadowIntensity((cameraInfo.info.view * vec4(inWorldPosition, 1)).xyz, wNormal);

    ColorAttachment = vec4(radiance * rmo.z * (1 - shadowIntensity), 1);
}
