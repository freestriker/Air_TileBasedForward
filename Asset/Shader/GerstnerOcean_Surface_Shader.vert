#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Camera.glsl"
#include "Object.glsl"
#include "Light.glsl"

#ifndef PI
    #define PI (3.14159265358979)
#endif
#ifndef GRAVITY
    #define GRAVITY (9.8)
#endif

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoords;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;

layout(location = 0) out vec3 outWorldPosition;
layout(location = 1) out vec3 outWorldNormal;

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
    uint count;
    float time;
} constantInfo;

vec3 CalculateGerstnerWave (
    const float time,
    const float amplitudeFactor,
    const vec2 windDirection,
    const float waveLength,
    const float phiRadian,   
	const vec3 worldPosition, 
    inout vec3 tangent, inout vec3 binormal
) 
{
    const float k = 2 * PI / waveLength;
    const float phaseRadian = k * (dot(windDirection, worldPosition.xz) - sqrt(GRAVITY / k) * time) + phiRadian;
    const float amplitude = amplitudeFactor / k;

    tangent += vec3(
        -windDirection.x * windDirection.x * (amplitudeFactor * sin(phaseRadian)),
        +windDirection.x * (amplitudeFactor * cos(phaseRadian)),
        -windDirection.x * windDirection.y * (amplitudeFactor * sin(phaseRadian))
    );
    binormal += vec3(
        -windDirection.x * windDirection.y * (amplitudeFactor * sin(phaseRadian)),
        +windDirection.y * (amplitudeFactor * cos(phaseRadian)),
        -windDirection.y * windDirection.y * (amplitudeFactor * sin(phaseRadian))
    );
    return vec3(
        windDirection.x * (amplitude * cos(phaseRadian)),
        amplitude * sin(phaseRadian),
        windDirection.y * (amplitude * cos(phaseRadian))
    );
}

void main() 
{
    vec4 worldPosition = vec4(0);
    {   
        const vec2 texCoords = vec2(1) - vertexTexCoords;

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
    }

    vec3 displacement = vec3(0);
    vec3 tangent = vec3(0);
    vec3 binormal = vec3(0);
    {   
        const float time = constantInfo.time;
        for(uint subWaveIndex = 0; subWaveIndex < constantInfo.count; ++subWaveIndex)
        {
            const float amplitudeFactor = subGerstnerWaveInfosBuffer.subGerstnerWaveInfos[subWaveIndex].amplitudeFactor;
            const float windRotationRadian = subGerstnerWaveInfosBuffer.subGerstnerWaveInfos[subWaveIndex].windRotationAngle * 2 * PI / 360;
            const vec2 windDirection = vec2(cos(windRotationRadian), -sin(windRotationRadian));
            const float waveLength = subGerstnerWaveInfosBuffer.subGerstnerWaveInfos[subWaveIndex].waveLength;
            const float phiRadian = subGerstnerWaveInfosBuffer.subGerstnerWaveInfos[subWaveIndex].phiAngle * 2 * PI / 360;

            displacement += CalculateGerstnerWave(time, amplitudeFactor, windDirection, waveLength, phiRadian, worldPosition.xyz, tangent, binormal);
        }
    }

    worldPosition.xyz = worldPosition.xyz + displacement.xyz;

    gl_Position = cameraInfo.info.projection * cameraInfo.info.view * worldPosition;

    outWorldPosition = worldPosition.xyz;
    outWorldNormal = normalize(cross(binormal, tangent));
}
