#version 450
#extension GL_GOOGLE_include_directive: enable

#define PI (acos(-1.0))

layout(location = 0) out vec2 AccumulationAttachment;

layout(push_constant) uniform AccumulationInfo
{
    uint stepCount;
    uint sliceCount;
    uint sliceIndex;
    uint resolution;
} accumulationInfo;

vec2 Hammersley(in uint i, in uint N)
{
    uint bits = i;
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);

    return vec2(float(i)/float(N), float(bits) * 2.3283064365386963e-10);
}

vec3 ImportanceSample(in vec2 hammersley, in float roughness) 
{
    float a2 = roughness * roughness;

    float phi = 2.0 * PI * hammersley.x;
    float cosTheta = sqrt((1.0 - hammersley.y) / (1.0 + (a2 * a2 - 1.0) * hammersley.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 offset = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    return normalize(offset);
}

void main() 
{
    if(accumulationInfo.sliceIndex >= accumulationInfo.sliceCount) 
    {
        discard;
        return;
    }

    vec2 aPosition = gl_FragCoord.xy / vec2(accumulationInfo.resolution, accumulationInfo.resolution);
    float cosnv = aPosition.x;
    float roughness = aPosition.y;
    vec3 iView = vec3(sqrt(1.0 - cosnv * cosnv), 0, cosnv);

    vec2 parameter = vec2(0, 0);

    uint perSliceStepCount = (accumulationInfo.stepCount + accumulationInfo.sliceCount - 1) / accumulationInfo.sliceCount;
    uint startStepIndex = accumulationInfo.sliceIndex * perSliceStepCount;
    for(uint stepOffset = 0; stepOffset < perSliceStepCount; stepOffset++)
    {
        uint stepIndex = stepOffset + startStepIndex;

        if(stepIndex >= accumulationInfo.stepCount) break;

        vec2 hammersley = Hammersley(stepIndex, accumulationInfo.stepCount);
        vec3 H = ImportanceSample(hammersley, roughness);
        vec3 iLight = normalize(reflect(-iView, H));

        float cosnl = max(0, iLight.z);
        float cosnh = max(0, H.z);
        float coshv = max(0, dot(H, iView));

        if(cosnl > 0.0)
        {
            float geometry = 1;
            {
                float a = roughness;
                // float a = roughness * roughness;
                float k = (a * a) / 2.0;

                float inGeomery = cosnl / (cosnl * (1 - k) + k);
                float outGeomery = cosnv / (cosnv * (1 - k) + k);

                geometry = inGeomery * outGeomery;
            }
            geometry = geometry * coshv / (cosnh * cosnv);

            float partF = pow(1.0 - coshv, 5.0);

            parameter += vec2(geometry * (1 - partF), geometry * partF);
        }
    }

    AccumulationAttachment = parameter / float(accumulationInfo.stepCount);
}
