#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"
#include "Object.glsl"
#include "Light.glsl"

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoords;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;

layout(location = 0) out vec2 outTexCoords;
layout(location = 1) out vec3 outWorldPosition;
// layout(location = 2) out vec3 outWorldNormal;
// layout(location = 3) out vec3 outWorldTangent;
// layout(location = 4) out vec3 outWorldBitangent;

layout(set = 0, binding = 0) uniform _CameraInfo
{
    CameraInfo info;
} cameraInfo;
layout(set = 1, binding = 0) uniform MeshObjectInfo
{
    ObjectInfo info;
} meshObjectInfo;
#define MAX_ORTHER_LIGHT_COUNT 256
layout(set = 2, binding = 0) uniform LightInfos
{
    LightInfo ambientLightInfo;
    LightInfo mainLightInfo;
    int ortherLightCount;
    LightInfo[MAX_ORTHER_LIGHT_COUNT] ortherLightInfos;
} lightInfos;
layout(set = 3, binding = 0) uniform sampler2D displacementTexture;
layout(set = 4, binding = 0) uniform sampler2D normalTexture;

void main() 
{
    const vec2 texCoords = vec2(1) - vertexTexCoords;
    const vec3 displacement = texture(displacementTexture, texCoords).xyz;
    // const vec2 aPosition = (vertexPosition.xz - vec2(constantInfo.minVertexPosition)) / vec2(constantInfo.maxVertexPosition - constantInfo.minVertexPosition);
    // const vec3 displacement = texture(displacementTexture, aPosition).rgb * constantInfo.displacementFactor;
    const vec3 newVertexPosition = vertexPosition + displacement;
    gl_Position = PositionO2P(newVertexPosition, meshObjectInfo.info, cameraInfo.info);
    // vec3 worldNormal = DirectionO2W(vertexNormal, meshObjectInfo.info);
    vec3 worldPosition = PositionO2W(newVertexPosition, meshObjectInfo.info);
    // vec3 viewDirection = CameraWObserveDirection(worldPosition, cameraInfo.info);

    outTexCoords = texCoords;
    outWorldPosition = worldPosition;
    // outWorldNormal = worldNormal;
    // outWorldTangent = DirectionO2W(vertexTangent, meshObjectInfo.info);
    // outWorldBitangent = normalize(cross(outWorldNormal, outWorldTangent));
}
