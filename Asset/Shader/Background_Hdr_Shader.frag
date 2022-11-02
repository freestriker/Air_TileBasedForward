#version 450
#extension GL_GOOGLE_include_directive: enable

#include "BackgroundRendering.glsl"

layout(set = START_SET_INDEX + 0, binding = 0) uniform sampler2D backgroundTexture;

layout(location = 0) out vec4 ColorAttachment;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main() 
{
    vec2 iPosition = gl_FragCoord.xy * attachmentSizeInfo.texelSize;
    vec3 ndcPosition = IMAGE_TO_NDC_SPACE(iPosition, 1);
    vec3 wPosition = PositionN2W(ndcPosition, cameraInfo.info);
    vec3 observeDirection = VIEW_DIRECTION(cameraInfo.info, wPosition);
    vec3 d = vec3(observeDirection.x, observeDirection.z, -observeDirection.y);
    vec2 uv = SampleSphericalMap(d);
    vec3 hdrColor = texture(backgroundTexture, uv).rgb;
    hdrColor = hdrColor / (vec3(1) + hdrColor);
    ColorAttachment = vec4(hdrColor, 1);
}
