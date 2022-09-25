#version 450
#extension GL_GOOGLE_include_directive: enable

#include "CSM_ShadowCaster.glsl"

layout(location = 0) in vec3 vertexPosition;

void main() 
{
    gl_Position = lightCameraInfo.viewProjection * meshObjectInfo.info.model * vec4(vertexPosition, 1);
}
