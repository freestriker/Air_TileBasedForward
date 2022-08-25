#version 450
#extension GL_GOOGLE_include_directive: enable

#include "Utils/BlurAmbientOcclusion.glsl"
#include "Utils/Object.glsl"

layout(location = 0) in vec2 texcoordOffset;

layout(location = 0) out float OcclusionAttachment;

void main()
{
    vec2 aPosition = gl_FragCoord.xy * blurInfo.attachmentTexelSize;
    vec3 vNormal = ParseFromColor(texture(normalTexture, aPosition).rgb);

    float occlusion = 0;
    float weightSum = 0;

    occlusion += texture(occlusionTexture, aPosition).r * GAUSSION_WEIGHT[0];
    weightSum += GAUSSION_WEIGHT[0];


    for(int i = 1; i < MAX_GAUSSION_KERNAL_RADIUS; i++)
    {
        vec2 offsetAPosition = aPosition + texcoordOffset * i;
        if(0 < offsetAPosition.x && offsetAPosition.x < 1 && 0 < offsetAPosition.y && offsetAPosition.y < 1)
        {
            vec3 offsetVNormal = ParseFromColor(texture(normalTexture, offsetAPosition).rgb);
            float weight = GAUSSION_WEIGHT[i] * smoothstep(0.1, 0.8, dot(offsetVNormal, vNormal));

            occlusion += texture(occlusionTexture, offsetAPosition).r * weight;
            weightSum += weight;
        }

        offsetAPosition = aPosition - texcoordOffset * i;
        if(0 < offsetAPosition.x && offsetAPosition.x < 1 && 0 < offsetAPosition.y && offsetAPosition.y < 1)
        {
            vec3 offsetVNormal = ParseFromColor(texture(normalTexture, offsetAPosition).rgb);
            float weight = GAUSSION_WEIGHT[i] * smoothstep(0.1, 0.8, dot(offsetVNormal, vNormal));

            occlusion += texture(occlusionTexture, offsetAPosition).r * weight;
            weightSum += weight;
        }
    }

    OcclusionAttachment = occlusion / weightSum;
}
