#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Common.glsl"
#include "Camera.glsl"
#include "Light.glsl"

layout(set = START_SET + 0, binding = 0) uniform sampler2D albedo;
layout(set = START_SET + 0, binding = 1) uniform AlbedoInfo{
    vec4 size;
	vec4 tilingScale;
} albedoInfo;

layout(set = START_SET + 1, binding = 0) uniform sampler2D normalTexture;
layout(set = START_SET + 1, binding = 1) uniform NormalTextureInfo{
    vec4 size;
	vec4 tilingScale;
} normalTextureInfo;

layout(location = 0) in vec2 inTexCoords;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec3 inWorldPosition;
layout(location = 3) in vec3 inWorldNormal;
layout(location = 4) in vec3 inWorldTangent;
layout(location = 5) in vec3 inWorldBitangent;

layout(location = 0) out vec4 colorAttachment;

void main() {
    vec3 wDisturbance = TBNMatrix(inWorldTangent, inWorldBitangent, inWorldNormal) * NormalColorToTangent(texture(normalTexture, inTexCoords));
    vec3 worldNormal = normalize(normalize(inWorldNormal) + wDisturbance);

    vec3 viewDirection = CameraWorldViewToPosition(inWorldPosition);

    vec3 environment = vec3(0, 0, 0);
    vec3 diffuse = vec3(0, 0, 0);
    vec3 specular = vec3(0, 0, 0);

    environment += SkyBoxLighting(normalize(reflect(viewDirection, worldNormal)));
    diffuse += DiffuseLighting(mainLight.light, worldNormal, inWorldPosition);
    specular += SpecularLighting(mainLight.light, viewDirection, inWorldPosition, worldNormal, 50.0);
    for(int i = 0; i < 4; i++)
    {
        diffuse += DiffuseLighting(importantLight.lights[i], worldNormal, inWorldPosition);
        specular += SpecularLighting(importantLight.lights[i], viewDirection, inWorldPosition, worldNormal, 80.0);
    }

    colorAttachment = texture(albedo, inTexCoords) * vec4(diffuse + specular + environment + inColor.xyz, 1);
}
