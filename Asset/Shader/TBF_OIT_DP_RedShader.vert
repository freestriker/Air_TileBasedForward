#version 450
#extension GL_GOOGLE_include_directive: enable

#include "TBF_OIT_DepthPeelingLighting.glsl"

layout(location = 0) in vec3 vertexPosition;

void main() 
{
    gl_Position = PositionO2P(vertexPosition, meshObjectInfo.info, cameraInfo.info);
}
