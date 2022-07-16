#version 450
#extension GL_GOOGLE_include_directive: enable

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexCoords;

layout(location = 0) out vec2 outTexCoords;

void main() 
{
    gl_Position = vec4(vertexPosition.xyz, 1.0f);
    outTexCoords = vertexTexCoords;
}
