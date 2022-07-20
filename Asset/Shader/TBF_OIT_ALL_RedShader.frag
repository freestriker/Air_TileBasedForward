#version 450
#extension GL_GOOGLE_include_directive: enable

#define FRAGMENT_STAGE
#include "TBF_OIT_AlphaLinkedListLighting.glsl"

void main() 
{
    vec4 color = vec4(1, 0, 0, 0.5);

    AddColorToAlphaLinkedList(color);
}
