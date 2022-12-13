#version 450
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_shader_atomic_float: enable

#include "TBForwardLighting.glsl"

layout(set = START_SET_INDEX + 0, binding = 0) uniform sampler2D albedoTexture;
// layout(set = START_SET_INDEX + 1, binding = 0) uniform sampler2D normalTexture;
layout(set = START_SET_INDEX + 1, binding = 0) uniform sampler2D rmoTexture;

layout(location = 0) in vec2 inTexCoords;
layout(location = 1) in vec3 inWorldPosition;
layout(location = 2) in vec3 inWorldNormal;
layout(location = 3) in vec3 inWorldTangent;
layout(location = 4) in vec3 inWorldBitangent;

layout(location = 0) out vec4 ColorAttachment;

void main() 
{
    // vec3 wDisturbance = TBNMatrix(inWorldTangent, inWorldBitangent, inWorldNormal) * NormalC2T(texture(normalTexture, inTexCoords));
    // vec3 wNormal = normalize(normalize(inWorldNormal) + wDisturbance);
    vec3 wNormal = normalize(inWorldNormal);
    vec3 wView = CameraWObserveDirection(inWorldPosition, cameraInfo.info);
    vec3 albedo = texture(albedoTexture, inTexCoords).rgb;
    vec3 rmo = texture(rmoTexture, inTexCoords).rgb;

    vec3 radiance = vec3(0, 0, 0);

    radiance += PbrLighting(lightInfos.mainLightInfo, inWorldPosition, wView, wNormal, albedo, rmo.x, rmo.y);
    for(int i = 0; i < opaqueLightIndexList.count; i++)
    {
        radiance += PbrLighting(lightInfos.ortherLightInfos[opaqueLightIndexList.indexes[i]], inWorldPosition, wView, wNormal, albedo, rmo.x, rmo.y);
    }

    vec3 iblRadiance;
    PBR_IBL_LIGHTING(iblRadiance, lightInfos.ambientLightInfo, inWorldPosition, wView, wNormal, albedo, rmo.x, rmo.y, irradianceCubeImage);
    radiance += iblRadiance;

    float shadowIntensity = GetShadowIntensity((cameraInfo.info.view * vec4(inWorldPosition, 1)).xyz, wNormal);
    
    ColorAttachment = vec4(radiance * rmo.z * (1 - shadowIntensity), 1);
}
