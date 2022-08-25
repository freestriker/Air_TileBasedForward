#version 450
#extension GL_GOOGLE_include_directive: enable

#include "TBF_OIT_DepthPeelingLighting.glsl"

layout(set = START_SET_INDEX + 0, binding = 0) uniform sampler2D diffuseTexture;

layout(location = 0) in vec2 inTexCoords;
layout(location = 1) in vec3 inWorldPosition;
layout(location = 2) in vec3 inWorldNormal;

layout(location = 0) out vec4 ColorAttachment;

void main() 
{
    DISCARD_PEELED_LAYER

    vec3 worldNormal = normalize(inWorldNormal);
    vec3 viewDirection = CameraWObserveDirection(inWorldPosition, cameraInfo.info);

    vec3 ambient = vec3(0, 0, 0);
    vec3 diffuse = vec3(0, 0, 0);
    vec3 specular = vec3(0, 0, 0);

    ambient += AmbinentLighting(normalize(reflect(viewDirection, worldNormal)));
    diffuse += DiffuseLighting(lightInfos.mainLightInfo, worldNormal, inWorldPosition);
    specular += SpecularLighting(lightInfos.mainLightInfo, viewDirection, inWorldPosition, worldNormal, 50.0);
    for(int i = 0; i < transparentLightIndexList.count; i++)
    {
        diffuse += DiffuseLighting(lightInfos.ortherLightInfos[transparentLightIndexList.indexes[i]], worldNormal, inWorldPosition);
        specular += SpecularLighting(lightInfos.ortherLightInfos[transparentLightIndexList.indexes[i]], viewDirection, inWorldPosition, worldNormal, 80.0);
    }

    ColorAttachment = texture(diffuseTexture, inTexCoords) * vec4(diffuse + specular + ambient * 0.5, 1);
}
