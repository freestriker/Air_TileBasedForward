#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"
#include "Object.glsl"

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoords;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;

layout(location = 0) out vec2 outTexCoords;
// layout(location = 1) out vec3 outWorldPosition;
// layout(location = 2) out vec3 outWorldNormal;
// layout(location = 3) out vec3 outWorldTangent;
// layout(location = 4) out vec3 outWorldBitangent;

layout(push_constant) uniform SurfaceConstantInfo
{
    ivec2 minVertexPosition;
    ivec2 maxVertexPosition;
    vec3 displacementFactor;
} constantInfo;

layout(set = 0, binding = 0) uniform _CameraInfo
{
    CameraInfo info;
} cameraInfo;

layout(set = 1, binding = 0) uniform MeshObjectInfo
{
    ObjectInfo info;
} meshObjectInfo;
layout(set = 2, binding = 0) uniform sampler2D displacementTexture;

void main() 
{
    const vec2 aPosition = (vertexPosition.xz - vec2(constantInfo.minVertexPosition)) / vec2(constantInfo.maxVertexPosition - constantInfo.minVertexPosition);
    const vec3 displacement = texture(displacementTexture, aPosition).rgb * constantInfo.displacementFactor;
    gl_Position = PositionO2P(vertexPosition + displacement, meshObjectInfo.info, cameraInfo.info);
    // vec3 worldNormal = DirectionO2W(vertexNormal, meshObjectInfo.info);
    // vec3 worldPosition = PositionO2W(vertexPosition, meshObjectInfo.info);
    // vec3 viewDirection = CameraWObserveDirection(worldPosition, cameraInfo.info);

    outTexCoords = vertexTexCoords;
    // outWorldPosition = worldPosition;
    // outWorldNormal = worldNormal;
    // outWorldTangent = DirectionO2W(vertexTangent, meshObjectInfo.info);
    // outWorldBitangent = normalize(cross(outWorldNormal, outWorldTangent));
}
