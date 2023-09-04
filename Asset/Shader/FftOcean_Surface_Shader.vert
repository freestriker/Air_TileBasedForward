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

layout(set = 0, binding = 0) uniform _CameraInfo
{
    CameraInfo info;
} cameraInfo;
#define MAX_ORTHER_LIGHT_COUNT 256
layout(set = 1, binding = 0) uniform LightInfos
{
    LightInfo ambientLightInfo;
    LightInfo mainLightInfo;
    int ortherLightCount;
    LightInfo[MAX_ORTHER_LIGHT_COUNT] ortherLightInfos;
} lightInfos;
layout(set = 2, binding = 0) uniform sampler2D displacementTexture;
layout(set = 3, binding = 0) uniform sampler2D normalTexture;

layout(push_constant) uniform ProjectedGridConstantInfo
{
    vec4 corner00;
    vec4 corner10;
    vec4 corner01;
    vec4 corner11;
    vec3 scale;
} constantInfo;

void main() 
{
    const vec2 texCoords = vec2(1) - vertexTexCoords;

    vec4 worldPosition;
    worldPosition.xzw = 
        (1.0 - texCoords.y) * 
            (
                (1.0 - texCoords.x) * constantInfo.corner00.xzw + 
                texCoords.x * constantInfo.corner10.xzw
            ) + 
        texCoords.y * 
            (
                (1.0 - texCoords.x) * constantInfo.corner01.xzw + 
                texCoords.x * constantInfo.corner11.xzw
            );
    worldPosition.xzw /= worldPosition.w;

    vec2 worldTexCoords;
    {
        const vec2 offset = mod(worldPosition.xz, constantInfo.scale.xz);
        worldTexCoords = offset / constantInfo.scale.xz;
    }

    const vec3 displacement = texture(displacementTexture, worldTexCoords).xyz * constantInfo.scale;
    worldPosition.y = displacement.y;
    //worldPosition.xz = worldPosition.xz + displacement.xz;

    // const vec3 newVertexPosition = vertexPosition + displacement;
    // const vec3 worldPosition = constantInfo.scale * newVertexPosition;
    gl_Position = cameraInfo.info.projection * cameraInfo.info.view * worldPosition;

    outTexCoords = worldTexCoords;
    outWorldPosition = vec3(worldPosition);
}
