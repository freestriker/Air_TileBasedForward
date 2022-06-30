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

layout(location = 0) in vec2 inTexCoords;

layout(location = 0) out vec4 colorAttachment;

void main() 
{
    colorAttachment = texture(albedo, inTexCoords);
}
