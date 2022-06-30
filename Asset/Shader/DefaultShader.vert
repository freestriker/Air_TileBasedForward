#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Common.glsl"
#include "Camera.glsl"
#include "Light.glsl"

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoords;
layout(location = 2) in vec3 vertexNormal;
layout(location = 3) in vec3 vertexTangent;
layout(location = 4) in vec3 vertexBitangent;

layout(location = 0) out vec2 outTexCoords;
layout(location = 1) out vec4 outColor;
layout(location = 2) out vec3 outWorldPosition;
layout(location = 3) out vec3 outWorldNormal;
layout(location = 4) out vec3 outWorldTangent;
layout(location = 5) out vec3 outWorldBitangent;

void main() {
    gl_Position = PositionObjectToProjection(vec4(vertexPosition, 1.0));
    vec3 worldNormal = normalize(DirectionObjectToWorld(vertexNormal));
    vec3 worldPosition = PositionObjectToWorld(vec4(vertexPosition, 1.0)).xyz;
    vec3 viewDirection = CameraWorldViewToPosition(worldPosition);

    vec3 diffuse = vec3(0, 0, 0);
    vec3 specular = vec3(0, 0, 0);
    for(int i = 0; i < 4; i++)
    {
        diffuse += DiffuseLighting(unimportantLight.lights[i], worldNormal, worldPosition);
        specular += SpecularLighting(unimportantLight.lights[i], viewDirection, worldPosition, worldNormal, 50.0);
    }

    outTexCoords = vertexTexCoords;
    outColor = vec4(diffuse.xyz + specular.xyz, 1);
    outWorldPosition = worldPosition;
    outWorldNormal = worldNormal;
    outWorldTangent = normalize(DirectionObjectToWorld(vertexTangent));
    outWorldBitangent = normalize(DirectionObjectToWorld(vertexBitangent));
}
