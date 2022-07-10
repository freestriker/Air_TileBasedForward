#ifndef _LIGHT_GLSL_
#define _LIGHT_GLSL_

#define INVALID_LIGHT 0
#define DIRECTIONL_LIGHT 1
#define POINT_LIGHT 2
#define SKYBOX_LIGHT 3
#define SPOT_LIGHT 4

#define FORWARD_LIGHTING 0
#define TILE_BASED_FORWARD_LIGHTING 1

#define PI 3.1415926535
#define ANGLE_TO_RADIANS (3.1415926535 / 180.0)
#define RADIANS_TO_ANGLE (180.0 / 3.1415926535)

struct LightInfo
{
    int type;
    float intensity;
    float minRange;
    float maxRange;
    vec4 extraParameter;
    vec3 position;
    vec3 direction;
    vec4 color;
};

struct LightBoundingBoxInfo
{
    vec3[8] vertexes;
};

vec3 DiffuseDirectionalLighting(in LightInfo lightInfo, in vec3 worldNormal)
{
    vec4 color = lightInfo.intensity * lightInfo.color * max(0, dot(normalize(worldNormal), -normalize(lightInfo.direction)));
    return color.xyz;
}

vec3 SpecularDirectionalLighting(in LightInfo lightInfo, in vec3 worldView, in vec3 worldNormal, in float gloss)
{
    vec3 worldReflect = normalize(reflect(normalize(lightInfo.direction), normalize(worldNormal)));
    vec3 inverseWorldView = normalize(-worldView);
    vec4 color = lightInfo.intensity * lightInfo.color * pow(max(0, dot(worldReflect, inverseWorldView)), gloss);
    return color.xyz;
}

vec3 DiffusePointLighting(in LightInfo lightInfo, in vec3 worldNormal, in vec3 worldPosition)
{
    vec3 lightingDirection = normalize(worldPosition - lightInfo.position);
    float d = distance(lightInfo.position, worldPosition);
    float k1 = 1.0 / max(lightInfo.minRange, d);
    float disAttenuation = k1 * k1;

    float win = pow(max(1 - pow(d / lightInfo.maxRange, 4), 0), 2);
    
    vec4 color = lightInfo.intensity * disAttenuation * win * lightInfo.color * max(0, dot(worldNormal, -lightingDirection));
    return color.xyz;
}

vec3 SpecularPointLighting(in LightInfo lightInfo, in vec3 worldView, in vec3 worldPosition, in vec3 worldNormal, in float gloss)
{
    vec3 worldReflect = normalize(reflect(normalize(worldPosition - lightInfo.position), worldNormal));
    vec3 inverseWorldView = normalize(-worldView);

    vec3 lightingDirection = normalize(worldPosition - lightInfo.position);
    float d = distance(lightInfo.position, worldPosition);
    float k1 = 1.0 / max(lightInfo.minRange, d);
    float disAttenuation = k1 * k1;

    float win = pow(max(1 - pow(d / lightInfo.maxRange, 4), 0), 2);
    
    vec4 color = lightInfo.intensity * disAttenuation * win * lightInfo.color * pow(max(0, dot(worldReflect, inverseWorldView)), gloss);
    return color.xyz;
}

vec3 DiffuseSpotLighting(in LightInfo lightInfo, in vec3 worldNormal, in vec3 worldPosition)
{
    vec3 lightingDirection = normalize(worldPosition - lightInfo.position);
    float d = distance(lightInfo.position, worldPosition);
    float k1 = 1.0 / max(lightInfo.minRange, d);
    float disAttenuation = k1 * k1;

    float win = pow(max(1 - pow(d / lightInfo.maxRange, 4), 0), 2);

    float innerCos = cos(ANGLE_TO_RADIANS * lightInfo.extraParameter.x);
    float outerCos = cos(ANGLE_TO_RADIANS * lightInfo.extraParameter.y);
    float dirCos = dot(normalize(lightInfo.direction), lightingDirection);
    float t = clamp((dirCos - outerCos) / (innerCos - outerCos), 0.0, 1.0);
    float dirAttenuation = t * t;

    vec4 color = lightInfo.intensity * dirAttenuation * disAttenuation * win * lightInfo.color * max(0, dot(worldNormal, -lightingDirection));
    return color.xyz;
}

vec3 SpecularSpotLighting(in LightInfo lightInfo, in vec3 worldView, in vec3 worldPosition, in vec3 worldNormal, in float gloss)
{
    vec3 worldReflect = normalize(reflect(normalize(worldPosition - lightInfo.position), worldNormal));
    vec3 inverseWorldView = normalize(-worldView);

    vec3 lightingDirection = normalize(worldPosition - lightInfo.position);
    float d = distance(lightInfo.position, worldPosition);
    float k1 = 1.0 / max(lightInfo.minRange, d);
    float disAttenuation = k1 * k1;

    float win = pow(max(1 - pow(d / lightInfo.maxRange, 4), 0), 2);

    float innerCos = cos(ANGLE_TO_RADIANS * lightInfo.extraParameter.x);
    float outerCos = cos(ANGLE_TO_RADIANS * lightInfo.extraParameter.y);
    float dirCos = dot(normalize(lightInfo.direction), lightingDirection);
    float t = clamp((dirCos - outerCos) / (innerCos - outerCos), 0.0, 1.0);
    float dirAttenuation = t * t;

    vec4 color = lightInfo.intensity * dirAttenuation * disAttenuation * win * lightInfo.color * pow(max(0, dot(worldReflect, inverseWorldView)), gloss);
    return color.xyz;
}

vec3 DiffuseLighting(in LightInfo lightInfo, in vec3 worldNormal, in vec3 worldPosition)
{
    switch(lightInfo.type)
    {
        case INVALID_LIGHT:
        {
            return vec3(0, 0, 0);
        }
        case DIRECTIONL_LIGHT:
        {
            return DiffuseDirectionalLighting(lightInfo, worldNormal);
        }
        case POINT_LIGHT:
        {
            return DiffusePointLighting(lightInfo, worldNormal, worldPosition);
        }
        case SPOT_LIGHT:
        {
            return DiffuseSpotLighting(lightInfo, worldNormal, worldPosition);
        }

    }
    return vec3(0, 0, 0);
}

vec3 SpecularLighting(in LightInfo lightInfo, in vec3 worldView, in vec3 worldPosition, in vec3 worldNormal, in float gloss)
{
    switch(lightInfo.type)
    {
        case INVALID_LIGHT:
        {
            return vec3(0, 0, 0);
        }
        case DIRECTIONL_LIGHT:
        {
            return SpecularDirectionalLighting(lightInfo, worldView, worldNormal, gloss);
        }
        case POINT_LIGHT:
        {
            return SpecularPointLighting(lightInfo, worldView, worldPosition, worldNormal, gloss);
        }
        case SPOT_LIGHT:
        {
            return SpecularSpotLighting(lightInfo, worldView, worldPosition, worldNormal, gloss);
        }
    }
    return vec3(0, 0, 0);
}

#endif