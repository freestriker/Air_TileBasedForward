#ifndef _FORWARD_LIGHTING_GLSL_
#define _FORWARD_LIGHTING_GLSL_

#include "Camera.glsl"
#include "Object.glsl"
#include "Light.glsl"

#ifndef LIGHTING_MODE
#define LIGHTING_MODE FORWARD_LIGHTING

#define START_SET_INDEX 4
#define MAX_ORTHER_LIGHT_COUNT 4

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
    int importantLightCount;
    LightInfo[MAX_ORTHER_LIGHT_COUNT] importantLightInfos;
    int unimportantLightCount;
    LightInfo[MAX_ORTHER_LIGHT_COUNT] unimportantLightInfos;
} lightInfos;

layout(set = 3, binding = 0) uniform samplerCube ambientLightTexture;

vec3 AmbinentLighting(in vec3 direction)
{
    vec4 color = lightInfos.ambientLightInfo.intensity * lightInfos.ambientLightInfo.color * texture(ambientLightTexture, normalize(direction));
    return color.rgb;
}

#endif ///#ifndef LIGHTING_MODE

#endif ///#ifndef _FORWARD_LIGHTING_GLSL_