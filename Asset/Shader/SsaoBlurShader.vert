#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Utils/BlurAmbientOcclusion.glsl"

layout(location = 0) in vec3 vertexPosition;

layout(location = 0) out vec2 texcoordOffset;

void main()
{
    gl_Position = vec4(vertexPosition.xy, 0.0, 1.0);

    texcoordOffset = blurInfo.sampleOffset * blurInfo.attachmentTexelSize;
}
