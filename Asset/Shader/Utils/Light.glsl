#ifndef _LIGHT_GLSL_
#define _LIGHT_GLSL_

#define INVALID_LIGHT 0
#define DIRECTIONL_LIGHT 1
#define POINT_LIGHT 2

struct Light
{
    int type;
    float intensity;
    float range;
    float extraData;
    vec3 position;
    vec4 color;
};

layout(set = 2, binding = 0) uniform samplerCube skyBoxTexture;

layout(set = 3, binding = 0) uniform SkyBox {
    Light light;
}skyBox;

layout(set = 4, binding = 0) uniform MainLight {
    Light light;
}mainLight;

layout(set = 5, binding = 0) uniform ImportantLight{
    Light lights[4];
} importantLight;

layout(set = 6, binding = 0) uniform UnimportantLight{
    Light lights[4];
} unimportantLight;

vec3 SkyBoxLighting(vec3 direction)
{
    vec4 color  = skyBox.light.intensity * skyBox.light.color * texture(skyBoxTexture, normalize(direction));
    return color.xyz;
}

vec3 DiffuseDirectionalLighting(Light light, vec3 worldNormal)
{
    vec4 color = light.intensity * light.color * max(0, dot(normalize(worldNormal), -normalize(light.position)));
    return color.xyz;
}

vec3 SpecularDirectionalLighting(Light light, vec3 worldView, vec3 worldNormal, float gloss)
{
    vec3 worldReflect = normalize(reflect(normalize(light.position), normalize(worldNormal)));
    vec3 inverseWorldView = normalize(-worldView);
    vec4 color = light.intensity * light.color * pow(max(0, dot(worldReflect, inverseWorldView)), gloss);
    return color.xyz;
}

vec3 DiffusePointLighting(Light light, vec3 worldNormal, vec3 worldPosition)
{
    vec3 lightDirection = normalize(worldPosition - light.position);
    float d = distance(light.position, worldPosition);
    float k1 = light.range / max(light.range, d);
    float attenuation = k1 * k1;
    float win = pow(max(1 - pow(d / light.extraData, 4), 0), 2);
    vec4 color = light.intensity * attenuation * win * light.color * max(0, dot(worldNormal, -lightDirection));
    return color.xyz;
}

vec3 SpecularPointLighting(Light light, vec3 worldView, vec3 worldPosition, vec3 worldNormal, float gloss)
{
    vec3 worldReflect = normalize(reflect(normalize(worldPosition - light.position), worldNormal));
    vec3 inverseWorldView = normalize(-worldView);
    vec4 color = light.intensity * light.color * pow(max(0, dot(worldReflect, inverseWorldView)), gloss);
    return color.xyz;
}

vec3 DiffuseLighting(Light light, vec3 worldNormal, vec3 worldPosition)
{
    switch(light.type)
    {
        case INVALID_LIGHT:
        {
            return vec3(0, 0, 0);
        }
        case DIRECTIONL_LIGHT:
        {
            return DiffuseDirectionalLighting(light, worldNormal);
        }
        case POINT_LIGHT:
        {
            return DiffusePointLighting(light, worldNormal, worldPosition);
        }
    }
    return vec3(0, 0, 0);
}

vec3 SpecularLighting(Light light, vec3 worldView, vec3 worldPosition, vec3 worldNormal, float gloss)
{
    switch(light.type)
    {
        case INVALID_LIGHT:
        {
            return vec3(0, 0, 0);
        }
        case DIRECTIONL_LIGHT:
        {
            return SpecularDirectionalLighting(light, worldView, worldNormal, gloss);
        }
        case POINT_LIGHT:
        {
            return SpecularPointLighting(light, worldView, worldPosition, worldNormal, gloss);
        }
    }
    return vec3(0, 0, 0);
}

#endif