#ifndef _COMMON_GLSL_
#define _COMMON_GLSL_

#define START_SET 7

layout(set = 0, binding = 0) uniform MatrixData{
    mat4 model;
    mat4 view;
    mat4 projection;
    mat4 itModel;
} matrixData;

vec4 PositionObjectToProjection(vec4 position)
{
    return matrixData.projection * matrixData.view * matrixData.model * position;
}

vec4 PositionObjectToWorld(vec4 position)
{
    return matrixData.model * position;
}

vec3 DirectionObjectToWorld(vec3 direction)
{
    return normalize(mat3(matrixData.itModel) * direction);
}

vec3 NormalColorToTangent(vec4 normalColor)
{
    return (normalColor.xyz - vec3(0.5, 0.5, 0.5)) * 2;
}

mat3 TBNMatrix(vec3 tangent, vec3 bitangent, vec3 normal)
{
    return mat3(normalize(tangent), normalize(bitangent), normalize(normal));
}
#endif