#version 450
#extension GL_GOOGLE_include_directive: enable
// #extension GL_EXT_shader_atomic_float: enable

#define PI (acos(-1.0))

layout(location = 0) in vec3 worldPosition;

layout(location = 0) out vec4 ColorAttachment;

layout(push_constant) uniform PushConstantInfo
{
    uint stepCount;
    uint sliceCount;
    uint sliceIndex;
    uint resolution;
    uint faceIndex;
    float roughness;
    mat4 viewProjection;
} pushConstantInfo;

layout(set = 0, binding = 0) uniform samplerCube environmentImage;
// layout(set = 1, binding = 0) buffer Info
// {
//     float weight;
// }info;

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

vec3 ImportanceSample(in vec2 hammersley, in mat3 matrix, in float roughness) 
{
    float a2 = roughness * roughness;

    float phi = hammersley.x * 2.0 * PI;
    float cosTheta = sqrt((1.0 - hammersley.y) / (1.0 + (a2 * a2 - 1.0) * hammersley.y));
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

    vec3 offset = vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);

    return normalize(matrix * offset);
}

void main() 
{
    if(pushConstantInfo.sliceIndex >= pushConstantInfo.sliceCount) 
    {
        discard;
        return;
    }

    vec3 wNormal = normalize(worldPosition);
    vec3 wView = -wNormal;
    mat3 matrix;
    {
        vec3 xAxis, yAxis, zAxis;
        zAxis = wNormal;
        vec3 up = abs(wNormal.z) < 0.9 ? vec3(0.0, 1.0, 0.0) : vec3(1.0, 0.0, 0.0);
        xAxis = normalize(cross(up, zAxis));
        yAxis =  normalize(cross(zAxis, xAxis));
        matrix = mat3(xAxis, yAxis, zAxis);
    }

    vec3 prefilteredColor = vec3(0, 0, 0);
    float weight = 0;

    float saTexel  = 4.0 * PI / (6.0 * pushConstantInfo.resolution * pushConstantInfo.resolution);

    uint perSliceStepCount = (pushConstantInfo.stepCount + pushConstantInfo.sliceCount - 1) / pushConstantInfo.sliceCount;
    uint startStepIndex = pushConstantInfo.sliceIndex * perSliceStepCount;
    for(uint stepOffset = 0; stepOffset < perSliceStepCount; stepOffset++)
    {
        uint stepIndex = stepOffset + startStepIndex;

        if(stepIndex >= pushConstantInfo.stepCount) break;

        vec2 hammersley = Hammersley(stepIndex, pushConstantInfo.stepCount);
        vec3 H = ImportanceSample(hammersley, matrix, pushConstantInfo.roughness);
        vec3 wiLight = normalize(reflect(wView, H));

        float cosnl = dot(wNormal, wiLight);
        float cosnh = dot(wNormal, H);
        float coshv = dot(H, -wView);

        float mipmapLevel = 0;
        {
            float a2 = pushConstantInfo.roughness * pushConstantInfo.roughness;
            float cosTheta2 = cosnh * cosnh;
            float d = (cosTheta2 * (a2 - 1) + 1);
            float distribution = a2 / (PI * d * d);

            float pdf = distribution * cosnh / (4.0 * coshv) + 0.0001;

            float saSample = 1.0 / (float(pushConstantInfo.stepCount) * pdf + 0.0001);

            mipmapLevel = pushConstantInfo.roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel); 
        } 

        vec3 sampleDirection = vec3(wiLight.xy, -wiLight.z);
        if(cosnl > 0.0)
        {
            prefilteredColor += textureLod(environmentImage, sampleDirection, mipmapLevel).rgb * cosnl;
            weight += cosnl;
        }
    }

    // if(pushConstantInfo.faceIndex == 0 && int(gl_FragCoord.x) == 0 && int(gl_FragCoord.y) == 0)
    // {
    //     atomicAdd(info.weight, weight);
    // }

    ColorAttachment = vec4(prefilteredColor, weight);
}
