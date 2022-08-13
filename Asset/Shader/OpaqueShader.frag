#version 450
#extension GL_GOOGLE_include_directive: enable
#extension GL_EXT_shader_atomic_float: enable

#include "TileBasedForwardLighting.glsl"

layout(origin_upper_left) in vec4 gl_FragCoord;

layout(set = START_SET_INDEX + 0, binding = 0) uniform sampler2D diffuseTexture;
layout(set = START_SET_INDEX + 1, binding = 0) uniform sampler2D normalTexture;

layout(location = 0) in vec2 inTexCoords;
layout(location = 1) in vec3 inWorldPosition;
layout(location = 2) in vec3 inWorldNormal;
layout(location = 3) in vec3 inWorldTangent;
layout(location = 4) in vec3 inWorldBitangent;

layout(location = 0) out vec4 ColorAttachment;

void main() 
{
    vec3 wDisturbance = TBNMatrix(inWorldTangent, inWorldBitangent, inWorldNormal) * NormalC2T(texture(normalTexture, inTexCoords));
    vec3 worldNormal = normalize(normalize(inWorldNormal) + wDisturbance);

    vec3 viewDirection = CameraWObserveDirection(inWorldPosition, cameraInfo.info);

    vec3 ambient = vec3(0, 0, 0);
    vec3 diffuse = vec3(0, 0, 0);
    vec3 specular = vec3(0, 0, 0);

    ambient += AmbinentLighting(normalize(reflect(viewDirection, worldNormal)));
    diffuse += DiffuseLighting(lightInfos.mainLightInfo, worldNormal, inWorldPosition);
    specular += SpecularLighting(lightInfos.mainLightInfo, viewDirection, inWorldPosition, worldNormal, 50.0);
    // for(int i = 0; i < lightInfos.importantLightCount; i++)
    // {
    //     diffuse += DiffuseLighting(lightInfos.importantLightInfos[i], worldNormal, inWorldPosition);
    //     specular += SpecularLighting(lightInfos.importantLightInfos[i], viewDirection, inWorldPosition, worldNormal, 80.0);
    // }
    for(int i = 0; i < opaqueLightIndexList.count; i++)
    {
        diffuse += DiffuseLighting(lightInfos.ortherLightInfos[opaqueLightIndexList.indexes[i]], worldNormal, inWorldPosition);
        specular += SpecularLighting(lightInfos.ortherLightInfos[opaqueLightIndexList.indexes[i]], viewDirection, inWorldPosition, worldNormal, 80.0);
    }

    ColorAttachment = texture(diffuseTexture, inTexCoords) * vec4(diffuse + specular + ambient, 1);
}
