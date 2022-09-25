#version 450
#extension GL_GOOGLE_include_directive: enable

#include "TBForwardLighting.glsl"

layout(location = 0) in vec3 inWorldPosition;
layout(location = 1) in vec3 inWorldNormal;

layout(location = 0) out vec4 ColorAttachment;

layout(set = START_SET_INDEX + 0, binding = 0) uniform samplerCube backgroundTexture;

const float refractRatio = 1.0 / 1.52;

void main() 
{
    vec3 worldNormal = normalize(inWorldNormal);
    vec3 viewDirection = CameraWObserveDirection(inWorldPosition, cameraInfo.info);

    vec3 ambient = vec3(0, 0, 0);
    vec3 diffuse = vec3(0, 0, 0);
    vec3 specular = vec3(0, 0, 0);

    ambient += AmbinentLighting(normalize(reflect(viewDirection, worldNormal)));
    diffuse += DiffuseLighting(lightInfos.mainLightInfo, worldNormal, inWorldPosition);
    specular += SpecularLighting(lightInfos.mainLightInfo, viewDirection, inWorldPosition, worldNormal, 50.0);
    for(int i = 0; i < opaqueLightIndexList.count; i++)
    {
        diffuse += DiffuseLighting(lightInfos.ortherLightInfos[opaqueLightIndexList.indexes[i]], worldNormal, inWorldPosition);
        specular += SpecularLighting(lightInfos.ortherLightInfos[opaqueLightIndexList.indexes[i]], viewDirection, inWorldPosition, worldNormal, 80.0);
    }

    vec3 background = texture(backgroundTexture, normalize(reflect(viewDirection, worldNormal))).xyz;
    
    float shadowIntensity = GetShadowIntensity((cameraInfo.info.view * vec4(inWorldPosition, 1)).xyz, worldNormal);

    ColorAttachment = vec4(background + ambient * 0.2 + diffuse * 0.2 + specular * 0.2, 1) * (1 - shadowIntensity);
}
