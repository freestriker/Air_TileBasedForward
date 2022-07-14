#version 450
#extension GL_GOOGLE_include_directive: enable

#include "TileBasedForwardLighting.glsl"

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoords;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

layout(location = 0) out vec2 outTexCoords;
layout(location = 1) out vec3 outWorldPosition;
layout(location = 2) out vec3 outWorldNormal;
layout(location = 3) out vec3 outWorldTangent;
layout(location = 4) out vec3 outWorldBitangent;

void main() 
{
    gl_Position = PositionO2P(vertexPosition, meshObjectInfo.info, cameraInfo.info);
    vec3 worldNormal = DirectionO2W(vertexNormal, meshObjectInfo.info);
    vec3 worldPosition = PositionO2W(vertexPosition, meshObjectInfo.info);
    vec3 viewDirection = CameraWObserveDirection(worldPosition, cameraInfo.info);

    outTexCoords = vertexTexCoords;
    outWorldPosition = worldPosition;
    outWorldNormal = worldNormal;
    outWorldTangent = DirectionO2W(vertexTangent, meshObjectInfo.info);
    outWorldBitangent = DirectionO2W(vertexBitangent, meshObjectInfo.info);
}
