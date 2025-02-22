#ifndef _TBFORWARD_LIGHTING_GLSL_
#define _TBFORWARD_LIGHTING_GLSL_

#include "Camera.glsl"
#include "Object.glsl"
#include "Light.glsl"

#ifndef LIGHTING_MODE
#define LIGHTING_MODE TBFORWARD_LIGHTING

#define CSM_SHADOW_RECEIVER_DESCRIPTOR_START_INDEX 10
#include "CSM_ShadowReceiver.glsl"

#define TBFORWARD_LIGHTING_DESCRIPTOR_COUNT (10 + CSM_SHADOW_RECEIVER_DESCRIPTOR_COUNT)

#define START_SET_INDEX TBFORWARD_LIGHTING_DESCRIPTOR_COUNT

#define MAX_ORTHER_LIGHT_COUNT 256
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

layout(set = 3, binding = 0) uniform samplerCube irradianceCubeImage;
layout(set = 4, binding = 0) uniform samplerCube prefilteredCubeImage;
layout(set = 5, binding = 0) uniform usampler2D lutImage;

struct LightIndexList
{
	int count;
	int[MAX_LIGHT_INDEX_COUNT] indexes;
};
layout (set = 6, binding = 0) readonly buffer OpaqueLightIndexLists
{
	ivec2 tileCount;
    LightIndexList[] lists;
}opaqueLightIndexLists;

layout (set = 7, binding = 0) readonly buffer TransparentLightIndexLists
{
	ivec2 tileCount;
    LightIndexList[] lists;
}transparentLightIndexLists;

layout (set = 8, binding = 0) uniform sampler2D occlusionTexture;
layout (set = 9, binding = 0) uniform OcclusionInfo
{
    vec2 texelSize;
    float intensity;
    float power;
} occlusionInfo;

#define pixelOffset (ivec2(gl_FragCoord.xy))
#define TILE_ID (ivec2(gl_FragCoord.xy) / TILE_WIDTH)

#define OPAQUE_TILE_1D_ID (TILE_ID.y * opaqueLightIndexLists.tileCount.x + TILE_ID.x)
#define opaqueLightIndexList (opaqueLightIndexLists.lists[OPAQUE_TILE_1D_ID])

#define TRANSPARENT_TILE_1D_ID (TILE_ID.y * transparentLightIndexLists.tileCount.x + TILE_ID.x)
#define transparentLightIndexList (transparentLightIndexLists.lists[TRANSPARENT_TILE_1D_ID])

vec3 AmbinentLighting(in vec3 direction)
{
    vec4 color  = lightInfos.ambientLightInfo.intensity * lightInfos.ambientLightInfo.color * texture(irradianceCubeImage, normalize(direction));
    return color.xyz;
}

#endif ///#ifndef LIGHTING_MODE

#endif ///#ifndef _TBFORWARD_LIGHTING_GLSL_