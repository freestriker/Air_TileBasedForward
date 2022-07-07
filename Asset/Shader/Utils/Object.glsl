#ifndef _OBJECT_GLSL_
#define _OBJECT_GLSL_

struct ObjectInfo
{
    mat4 model;
    mat4 itModel;
    vec3[8] boundingBox;
};

vec4 PositionO2W(in vec3 position, in ObjectInfo objectInfo)
{
    return objectInfo.model * vec4(position, 1);
}

vec3 DirectionO2W(in vec3 direction, in ObjectInfo objectInfo)
{
    return normalize(mat3(objectInfo.itModel) * direction);
}

vec3 NormalC2T(in vec4 normalColor)
{
    return (normalColor.xyz - vec3(0.5, 0.5, 0.5)) * 2;
}

mat3 TBNMatrix(in vec3 tangent, in vec3 bitangent, in vec3 normal)
{
    return mat3(normalize(tangent), normalize(bitangent), normalize(normal));
}

#ifdef _CAMERA_GLSL_
vec4 PositionO2P(in vec3 position, in ObjectInfo objectInfo)
{
    return cameraInfo.projection * cameraInfo.view * objectInfo.model * vec4(position, 1);
}

vec4 PositionO2V(in vec3 position, in ObjectInfo objectInfo)
{
    return cameraInfo.view * objectInfo.model * vec4(position, 1);
}
#endif ///#ifdef _CAMERA_GLSL_

#endif ///#ifndef _OBJECT_GLSL_