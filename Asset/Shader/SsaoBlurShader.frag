#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Utils/BlurAmbientOcclusion.glsl"

layout(location = 0) in vec2 texcoordOffset;

layout(location = 0) out float OcclusionAttachment;

void main()
{
    vec2 aPosition = gl_FragCoord.xy * blurInfo.attachmentTexelSize;

    float occlusion = 0;

    occlusion += texture(occlusionTexture, aPosition).r * GAUSSION_WEIGHT[0];
    for(int i = 1; i < MAX_GAUSSION_KERNAL_RADIUS; i++)
    {
        vec2 offsetAPosition = aPosition + texcoordOffset * i;
        if(0 < offsetAPosition.x && offsetAPosition.x < 1 && 0 < offsetAPosition.y && offsetAPosition.y < 1)
        {
            occlusion += texture(occlusionTexture, offsetAPosition).r * GAUSSION_WEIGHT[i];
        }

        offsetAPosition = aPosition - texcoordOffset * i;
        if(0 < offsetAPosition.x && offsetAPosition.x < 1 && 0 < offsetAPosition.y && offsetAPosition.y < 1)
        {
            occlusion += texture(occlusionTexture, offsetAPosition).r * GAUSSION_WEIGHT[i];
        }
    }

    OcclusionAttachment = occlusion;
}
