#version 450
#extension GL_GOOGLE_include_directive: enable

layout(location = 0) out vec4 ColorAttachment;

layout(push_constant) uniform PushConstantInfo
{
    uint sliceCount;
    uint sliceIndex;
} pushConstantInfo;

layout(set = 0, binding = 0) readonly buffer WeightInfo
{
    float weight;
}weightInfo;

void main() 
{
    if(pushConstantInfo.sliceIndex == pushConstantInfo.sliceCount)
    {
        ColorAttachment = vec4(weightInfo.weight, weightInfo.weight, weightInfo.weight, 1);
    }
    else
    {
        discard;
        return;
    }
}
