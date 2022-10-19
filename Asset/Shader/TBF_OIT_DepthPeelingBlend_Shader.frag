#version 450
#extension GL_GOOGLE_include_directive: enable

layout(set = 0, binding = 0) uniform sampler2DArray colorTextureArray;
layout (push_constant) uniform BlendInfo
{
    vec2 texelSize;
    int depthPeelingStepCount;
} blendInfo;

layout(location = 0) out vec4 ColorAttachment;

void main() 
{
    vec4 dst = vec4(0, 0, 0, 1);
    vec4 src = vec4(0, 0, 0, 0);
    vec4 color = vec4(0, 0, 0, 0);

    vec2 aPosition = gl_FragCoord.xy * blendInfo.texelSize;

    for(int i = 0; i < blendInfo.depthPeelingStepCount; i++)
    {
        src = texture(colorTextureArray, vec3(aPosition, i));
        color = vec4(dst.a * src.a * src.rgb + dst.rgb, (1 - src.a) * dst.a);
        dst = color;
    }

    ColorAttachment = dst;
}
