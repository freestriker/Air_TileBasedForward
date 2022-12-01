#version 450
#extension GL_GOOGLE_include_directive: enable

#define PI (acos(-1.0))

layout(location = 0) in vec3 worldPosition;
layout(location = 0) out vec4 ColorAttachment;

layout(push_constant) uniform GenerateFragmentStageInfo
{
    uint stepCount;
    uint sliceCount;
    uint sliceIndex;
    mat4 viewProjection;
} generateInfo;

layout(set = 0, binding = 0) uniform samplerCube environmentImage;

float RadicalInverse_VdC(in uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

vec2 Hammersley(in uint i, in uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}

vec3 HemisphereSample(in float u, in float v) 
{
     float phi = v * 2.0 * PI;
     float cosTheta = 1.0 - u;
     float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
     return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}

void main() 
{
    if(generateInfo.sliceIndex >= generateInfo.sliceCount) 
    {
        ColorAttachment = vec4(0, 0, 0, 1);
        return;
    }

    vec3 viewDirection = normalize(worldPosition);
    viewDirection.z = -viewDirection.z;

    vec3 xAxis, yAxis, zAxis;
    // if(abs(viewDirection.z) == 1)
    // {
    //     zAxis = viewDirection; 
    //     yAxis = vec3(0, 1, 0);
    //     xAxis =  vec3(1, 0, 0);
    // }
    // else
    {
        zAxis = viewDirection;
        yAxis =  normalize(cross(zAxis, vec3(1, 0, 0)));
        xAxis = normalize(cross(yAxis, zAxis));
    }
    
    uint perSliceStepCount = generateInfo.stepCount / generateInfo.sliceCount;
    uint startStepIndex = generateInfo.sliceIndex * perSliceStepCount;
    // uint perSliceStepCount = 1;
    // uint startStepIndex = 0;
    vec3 irradiance = vec3(0, 0, 0);
    for(uint i = 0; i < perSliceStepCount; i++)
    {
        vec2 hammersley = Hammersley(startStepIndex + i, generateInfo.stepCount);
        vec3 displacement = HemisphereSample(hammersley.x, hammersley.y);
        vec3 sampleDirection = normalize(mat3(xAxis, yAxis, zAxis) * displacement);

        irradiance += texture(environmentImage, sampleDirection).rgb;
    }
    irradiance /= float(generateInfo.stepCount);

    ColorAttachment = vec4(irradiance, 1);
}
