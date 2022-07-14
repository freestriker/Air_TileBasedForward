#ifndef _TILE_BASED_FORWARD_LIGHTING_GLSL_
#define _TILE_BASED_FORWARD_LIGHTING_GLSL_

#include "Camera.glsl"
#include "Object.glsl"
#include "Light.glsl"

#ifndef LIGHTING_MODE
#define LIGHTING_MODE TILE_BASED_FORWARD_LIGHTING

#define START_SET_INDEX 5

#define MAX_ORTHER_LIGHT_COUNT 1024
#define MAX_LIGHT_INDEX_COUNT 64
#define TILE_WIDTH 32

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
	int count;
	int[MAX_LIGHT_INDEX_COUNT] indexes;
};
layout (set = 4, binding = 0) buffer LightIndexLists
{
	ivec2 tileCount;
    LightIndexList[] lists;
}lightIndexLists;

#define pixelOffset (ivec2(gl_FragCoord.xy))
#define TILE_ID (ivec2(gl_FragCoord.xy) / TILE_WIDTH)
#define TILE_1D_ID (TILE_ID.y * lightIndexLists.tileCount.x + TILE_ID.x)
#define lightIndexList (lightIndexLists.lists[TILE_1D_ID])

vec3 AmbinentLighting(in vec3 direction)
{
    vec4 color  = lightInfos.ambientLightInfo.intensity * lightInfos.ambientLightInfo.color * texture(ambientLightTexture, normalize(direction));
    return color.xyz;
}

#endif ///#ifndef LIGHTING_MODE

#endif ///#ifndef _TILE_BASED_FORWARD_LIGHTING_GLSL_