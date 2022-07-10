#ifndef _FORWARD_LIGHTING_GLSL_
#define _FORWARD_LIGHTING_GLSL_

#include "Camera.glsl"
#include "Object.glsl"
#include "Light.glsl"

#ifndef LIGHTING_MODE
#define LIGHTING_MODE TILE_BASED_FORWARD_LIGHTING

#define START_SET_INDEX 5
#define MAX_ORTHER_LIGHT_COUNT 1024
#define MAX_LIGHT_INDEX_LIST_SIZE = 64;

layout(set = 0, binding = 0) uniform _CameraInfo
{
    CameraInfo info;
} cameraInfo;

layout(set = 1, binding = 0) uniform MeshObjectInfo
{
    ObjectInfo info;
} meshObjectInfo;

layout(set = 2, binding = 0) uniform LightInfos
{
    LightInfo ambientLightInfo;
    LightInfo mainLightInfo;
    int ortherLightCount;
    LightInfo[MAX_ORTHER_LIGHT_COUNT] ortherLightInfos;
} lightInfos;

layout(set = 3, binding = 0) uniform samplerCube ambientLightTexture;

struct LightIndexList
{
	int size;
	int[MAX_LIGHT_INDEX_LIST_SIZE] indexes;
};
layout(set = 4, binding = 0) uniform LightIndexLists
{
    LightIndexList[] lists;
}lightIndexLists;


vec3 AmbinentLighting(in vec3 direction)
{
    vec4 color  = lightInfos.ambientLightInfo.intensity * lightInfos.ambientLightInfo.color * texture(ambientLightTexture, normalize(direction));
    return color.xyz;
}

#endif ///#ifndef LIGHTING_MODE

#endif ///#ifndef _FORWARD_LIGHTING_GLSL_