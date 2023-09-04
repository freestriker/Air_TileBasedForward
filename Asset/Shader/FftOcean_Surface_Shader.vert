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

// layout(set = 0, binding = 0) uniform _CameraInfo
// {
//     CameraInfo info;
// } cameraInfo;
// layout(set = 1, binding = 0) uniform MeshObjectInfo
// {
//     ObjectInfo info;
// } meshObjectInfo;
// #define MAX_ORTHER_LIGHT_COUNT 256
// layout(set = 2, binding = 0) uniform LightInfos
// {
//     LightInfo ambientLightInfo;
//     LightInfo mainLightInfo;
//     int ortherLightCount;
//     LightInfo[MAX_ORTHER_LIGHT_COUNT] ortherLightInfos;
// } lightInfos;
// layout(set = 3, binding = 0) uniform sampler2D displacementTexture;
// layout(set = 4, binding = 0) uniform sampler2D normalTexture;

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
    const vec3 displacement = texture(displacementTexture, texCoords).xyz;
    const vec3 newVertexPosition = vertexPosition + displacement;
    const vec3 worldPosition = constantInfo.scale * newVertexPosition;
    gl_Position = cameraInfo.info.projection * cameraInfo.info.view * vec4(worldPosition, 1);

    outTexCoords = texCoords;
    outWorldPosition = worldPosition;
}
