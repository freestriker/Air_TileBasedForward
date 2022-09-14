#ifndef _CSM_SHADOW_CASTER_GLSL_
#define _CSM_SHADOW_CASTER_GLSL_

#include "Object.glsl"

#define START_SET_INDEX 2

layout(set = 0, binding = 0) uniform MeshObjectInfo
{
    ObjectInfo info;
} meshObjectInfo;
layout(set = 1, binding = 0) uniform LightCameraInfo
{
    mat4 view;
    mat4 projection;
	mat4 viewProjection; 
} lightCameraInfo;

#endif ///#ifndef _CSM_SHADOW_CASTER_GLSL_