#version 450
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_shader_atomic_float: enable

#include "TBForwardLighting.glsl"

layout(location = 0) in vec2 inTexCoords;
layout(location = 1) in vec3 inWorldPosition;
layout(location = 2) in vec3 inWorldNormal;

layout(location = 0) out vec4 ColorAttachment;

void main() 
{
    vec3 wNormal = normalize(inWorldNormal);
    vec3 wView = CameraWObserveDirection(inWorldPosition, cameraInfo.info);
    vec3 rmo = vec3(0.00, 1.00, 1.00);
    vec3 albedo = vec3(1.0, 1.0, 1.0);

    float shadowIntensity = GetShadowIntensity((cameraInfo.info.view * vec4(inWorldPosition, 1)).xyz, wNormal);
    
    float occlusion = 0;
    {    
        vec2 aPosition = gl_FragCoord.xy * occlusionInfo.texelSize;
        occlusion = texture(occlusionTexture, aPosition).r * occlusionInfo.intensity;
        occlusion = pow(clamp(1.0f - occlusion, 0, 1), occlusionInfo.power);
    }    

    vec3 radiance = vec3(0, 0, 0);

    radiance += PbrLighting(lightInfos.mainLightInfo, inWorldPosition, wView, wNormal, albedo, rmo.x, rmo.y) * (1 - clamp(shadowIntensity * 6, 0, 1));
    for(int i = 0; i < opaqueLightIndexList.count; i++)
    {
        radiance += PbrLighting(lightInfos.ortherLightInfos[opaqueLightIndexList.indexes[i]], inWorldPosition, wView, wNormal, albedo, rmo.x, rmo.y);
    }

    vec3 iblRadiance = vec3(0, 0, 0);
    PBR_IBL_LIGHTING(iblRadiance, lightInfos.ambientLightInfo, inWorldPosition, wView, wNormal, albedo, rmo.x, rmo.y, irradianceCubeImage, prefilteredCubeImage, lutImage);
    radiance += iblRadiance * occlusion * rmo.z;

    ColorAttachment = vec4(radiance / (radiance + vec3(1)), 1);
}
