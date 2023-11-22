#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"
#include "Object.glsl"
#include "Light.glsl"

#define PI (3.14159265358979)
#define GRAVITY (9.8)


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
struct SubGerstnerWaveInfo
{
    float amplitudeFactor;
    float windRotationAngle;
    float waveLength;
    float phiAngle;
};
layout(set = 2, binding = 0) readonly buffer SubGerstnerWaveInfosBuffer
{
    SubGerstnerWaveInfo subGerstnerWaveInfos[];
}subGerstnerWaveInfosBuffer;

layout(push_constant) uniform ProjectedGridConstantInfo
{
    vec4 corner00;
    vec4 corner10;
    vec4 corner01;
    vec4 corner11;
    vec3 scale;
    uint count;
    float time;
} constantInfo;

vec3 CalculateGerstnerWave (
    const float amplitudeFactor,
    const vec2 windDirection,
    const float waveLength,
    const float phiRadian,   
	const vec3 worldPosition, 
    inout vec3 tangent, inout vec3 binormal
) 
{
    const float k = 2 * PI / waveLength;
    const float omega = sqrt(GRAVITY * k);
    float c = sqrt(9.8 / k);
    float f = k * (dot(d, p.xz) - c * _Time.y);
    const float amplitude = amplitudeFactor / k;

    tangent += float3(
        -d.x * d.x * (amplitudeFactor * sin(f)),
        d.x * (amplitudeFactor * cos(f)),
        -d.x * d.y * (amplitudeFactor * sin(f))
    );
    binormal += float3(
        -d.x * d.y * (amplitudeFactor * sin(f)),
        d.y * (amplitudeFactor * cos(f)),
        -d.y * d.y * (amplitudeFactor * sin(f))
    );
    return float3(
        d.x * (amplitude * cos(f)),
        amplitude * sin(f),
        d.y * (amplitude * cos(f))
    );
}

void main() 
{
    const vec2 texCoords = vec2(1) - vertexTexCoords;

    vec4 worldPosition = vec4(0);
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

    const vec2 worldTexCoords = worldPosition.xz / constantInfo.scale.xz;

    for(uint subWaveIndex = 0; subWaveIndex < subGerstnerWaveInfosBuffer.count; ++subGerstnerWaveInfosBuffer)
    {
        const float amplitudeFactor = subGerstnerWaveInfosBuffer.subGerstnerWaveInfos[subWaveIndex].amplitudeFactor;
        const float windRotationRadian = subGerstnerWaveInfosBuffer.subGerstnerWaveInfos[subWaveIndex].windRotationAngle * 2 * PI / 360;
        const vec2 windDirection = vec2(cos(windRotationRadian), -sin(windRotationRadian));
        const float waveLength = subGerstnerWaveInfosBuffer.subGerstnerWaveInfos[subWaveIndex].waveLength;
        const float phiRadian = subGerstnerWaveInfosBuffer.subGerstnerWaveInfos[subWaveIndex].phiAngle * 2 * PI / 360;

    }

    const vec3 displacement = texture(displacementTexture, worldTexCoords).xyz * constantInfo.scale;
    worldPosition.xyz = worldPosition.xyz + displacement.xyz;

    gl_Position = cameraInfo.info.projection * cameraInfo.info.view * worldPosition;

    outTexCoords = worldTexCoords;
    outWorldPosition = vec3(worldPosition);
}
