#version 450
#extension GL_GOOGLE_include_directive: enable

layout(location = 0) in vec3 worldPosition;
layout(location = 0) out vec4 ColorAttachment;

layout(set = 0, binding = 0) uniform sampler2D hdrTexture;

vec2 SampleSphericalMap(vec3 v)
{
    vec2 invAtan = vec2(0.1591, 0.3183);
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main() 
{
    vec3 observeDirection = normalize(worldPosition);
    observeDirection.z = -observeDirection.z;
    vec2 uv = SampleSphericalMap(observeDirection);
    vec3 hdrColor = texture(hdrTexture, uv).rgb;
    ColorAttachment = vec4(hdrColor, 1);
}
