#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"
#include "Object.glsl"
// #include "TBForwardLighting.glsl"

// layout(set = START_SET_INDEX + 0, binding = 0) uniform sampler2D albedoTexture;
// layout(set = START_SET_INDEX + 1, binding = 0) uniform sampler2D normalTexture;
// layout(set = START_SET_INDEX + 2, binding = 0) uniform sampler2D rmoTexture;

layout(location = 0) in vec2 inTexCoords;
// layout(location = 1) in vec3 inWorldPosition;
// layout(location = 2) in vec3 inWorldNormal;
// layout(location = 3) in vec3 inWorldTangent;
// layout(location = 4) in vec3 inWorldBitangent;

layout(location = 0) out vec4 ColorAttachment;

layout(set = 3, binding = 0) uniform sampler2D normalTexture;
layout(set = 4, binding = 0) uniform sampler2D albedoTexture;

void main() 
{
    // vec3 b = cross(normalize(inWorldBitangent), normalize(inWorldNormal));
    // vec3 t = cross(normalize(inWorldNormal), normalize(inWorldBitangent));
    // vec3 wNormal = normalize(TBNMatrix(t, b, inWorldNormal) * NormalC2T(texture(normalTexture, inTexCoords)));
    // // vec3 wNormal = normalize(normalize(inWorldNormal) + wDisturbance);
    // // vec3 wNormal = normalize(inWorldNormal);
    // vec3 wView = CameraWObserveDirection(inWorldPosition, cameraInfo.info);
    // vec3 albedo = texture(albedoTexture, inTexCoords).rgb;
    // vec3 rmo = texture(rmoTexture, inTexCoords).rgb;
    // // vec3 rmo = vec3(1, 0, 1);

    // float shadowIntensity = GetShadowIntensity((cameraInfo.info.view * vec4(inWorldPosition, 1)).xyz, wNormal);
    
    // float occlusion = 0;
    // {    
    //     vec2 aPosition = gl_FragCoord.xy * occlusionInfo.texelSize;
    //     occlusion = texture(occlusionTexture, aPosition).r * occlusionInfo.intensity;
    //     occlusion = pow(clamp(1.0f - occlusion, 0, 1), occlusionInfo.power);
    // }    

    // vec3 radiance = vec3(0, 0, 0);

    // radiance += PbrLighting(lightInfos.mainLightInfo, inWorldPosition, wView, wNormal, albedo, rmo.x, rmo.y) * (1 - clamp(shadowIntensity * 6, 0, 1));
    // for(int i = 0; i < opaqueLightIndexList.count; i++)
    // {
    //     radiance += PbrLighting(lightInfos.ortherLightInfos[opaqueLightIndexList.indexes[i]], inWorldPosition, wView, wNormal, albedo, rmo.x, rmo.y);
    // }
    // //radiance *= 0.1 * occlusion;

    // vec3 iblRadiance = vec3(0, 0, 0);
    // PBR_IBL_LIGHTING(iblRadiance, lightInfos.ambientLightInfo, inWorldPosition, wView, wNormal, albedo, rmo.x, rmo.y, irradianceCubeImage, prefilteredCubeImage, lutImage);
    // radiance += iblRadiance * occlusion * rmo.z;

    // ColorAttachment = vec4(radiance / (radiance + vec3(1)), 1);
    vec3 color = texture(albedoTexture, inTexCoords).rgb;
    vec3 normal = texture(normalTexture, inTexCoords).rgb;
    vec3 normalColor = ParseFromColor(normal);
    ColorAttachment = vec4(normalColor, 1);
    // ColorAttachment = vec4(normalize(inWorldNormal) * 0.5 + vec3(0.5), 1);
    // ColorAttachment = vec4(normalize(inWorldTangent) * 0.5 + vec3(0.5), 1);
    // ColorAttachment = vec4(normalize(inWorldBitangent) * 0.5 + vec3(0.5), 1);
}
