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

    mat3 matrix;
    {
        vec3 xAxis, yAxis, zAxis;
        zAxis = normalize(worldPosition);
        zAxis.z = - zAxis.z;
        yAxis =  normalize(cross(zAxis, vec3(1, 0, 0)));
        xAxis = normalize(cross(yAxis, zAxis));
        matrix = mat3(xAxis, yAxis, zAxis);
    }

    uint perSliceStepCount = (generateInfo.stepCount + generateInfo.sliceCount - 1) / generateInfo.sliceCount;
    uint startStepIndex = generateInfo.sliceIndex * perSliceStepCount;
    vec3 irradiance = vec3(0, 0, 0);
    for(uint stepOffset = 0; stepOffset < perSliceStepCount; stepOffset++)
    {
        uint stepIndex = stepOffset + startStepIndex;

        if(stepIndex >= generateInfo.stepCount) break;

        vec2 hammersley = Hammersley(stepIndex, generateInfo.stepCount);
        vec3 displacement = HemisphereSample(hammersley.x, hammersley.y);
        vec3 sampleDirection = normalize(matrix * displacement);
        
        vec3 intensity = texture(environmentImage, sampleDirection).rgb;
        irradiance += intensity;
    }
    irradiance /= float(generateInfo.stepCount);

    ColorAttachment = vec4(irradiance, 1);
}
