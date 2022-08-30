#version 450
#extension GL_GOOGLE_include_directive: enable

#include "ForwardLighting.glsl"

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec3 outWorldPosition;
layout(location = 2) out vec3 outWorldNormal;

void main() 
{
    gl_Position = PositionO2P(vertexPosition, meshObjectInfo.info, cameraInfo.info);
    vec3 worldNormal = DirectionO2W(vertexNormal, meshObjectInfo.info);
    vec3 worldPosition = PositionO2W(vertexPosition, meshObjectInfo.info);
    vec3 viewDirection = CameraWObserveDirection(worldPosition, cameraInfo.info);

    vec3 diffuse = vec3(0, 0, 0);
    vec3 specular = vec3(0, 0, 0);
    for(int i = 0; i < lightInfos.unimportantLightCount; i++)
    {
        diffuse += DiffuseLighting(lightInfos.unimportantLightInfos[i], worldNormal, worldPosition);
        specular += SpecularLighting(lightInfos.unimportantLightInfos[i], viewDirection, worldPosition, worldNormal, 50.0);
    }

    outColor = vec4(diffuse.xyz + specular.xyz, 1);
    outWorldPosition = worldPosition;
    outWorldNormal = worldNormal;
}
