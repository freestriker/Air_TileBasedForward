#version 450
#extension GL_GOOGLE_include_directive: enable

layout(set = 0, binding = 0) uniform BlendInfo
{
    vec2 attachmentSize;
    vec2 attachmentTexelSize;
} blendInfo;
layout(set = 1, binding = 0) uniform sampler2D occlusionTexture;

layout(location = 0) out vec4 ColorAttachment;

void main()
{
    vec2 aPosition = gl_FragCoord.xy * blendInfo.attachmentTexelSize;
    float occlusion = texture(occlusionTexture, aPosition).r;

    ColorAttachment = vec4(0, 0, 0, pow(clamp(1.0f - occlusion, 0, 1), 3));
}
