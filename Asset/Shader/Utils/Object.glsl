#ifdef OBJECT_INFO_SET_INDEX

#ifndef _OBJECT_GLSL_
#define _OBJECT_GLSL_

layout(set = OBJECT_INFO_SET_INDEX, binding = 0) uniform ObjectInfo
{
    mat4 model;
    mat4 itModel;
} objectInfo;

vec4 PositionO2W(vec3 position)
{
    return objectInfo.model * vec4(position, 1);
}

vec3 DirectionO2W(vec3 direction)
{
    return normalize(mat3(objectInfo.itModel) * direction);
}

vec3 NormalC2T(vec4 normalColor)
{
    return (normalColor.xyz - vec3(0.5, 0.5, 0.5)) * 2;
}

mat3 TBNMatrix(vec3 tangent, vec3 bitangent, vec3 normal)
{
    return mat3(normalize(tangent), normalize(bitangent), normalize(normal));
}

#ifdef _CAMERA_GLSL_
vec4 PositionO2P(vec3 position)
{
    return cameraInfo.projection * cameraInfo.view * objectInfo.model * vec4(position, 1);
}

vec4 PositionO2V(vec3 position)
{
    return matrixData.view * matrixData.model * vec4(position, 1);
}
#endif ///#ifdef _CAMERA_GLSL_

#endif ///#ifndef _OBJECT_GLSL_
#endif ///#ifdef OBJECT_INFO_SET_INDEX