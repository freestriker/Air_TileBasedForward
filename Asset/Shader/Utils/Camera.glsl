#ifndef _CAMERA_GLSL_
#define _CAMERA_GLSL_

#define ORTHOGRAPHIC_CAMERA 1
#define PERSPECTIVE_CAMERA 2

layout(set = 1, binding = 0) uniform CameraData{
    int type;
    float nearFlat;
    float farFlat;
    float aspectRatio;
    vec3 position;
    vec4 parameter;
    vec3 forward;
    vec3 right;
    vec4 clipPlanes[6];
} cameraData;

///NDC Z -> View space Z
float OrthographicCameraDepthNdcToView(float ndcDepth)
{
    return ndcDepth * (cameraData.nearFlat - cameraData.farFlat) - cameraData.nearFlat;
}

float PerspectiveCameraDepthNdcToView(float ndcDepth)
{
    return cameraData.nearFlat * cameraData.farFlat / (ndcDepth * (cameraData.farFlat - cameraData.nearFlat) - cameraData.farFlat);
}

float DepthNdcToView(float ndcDepth)
{
    switch(cameraData.type)
    {
        case ORTHOGRAPHIC_CAMERA:
        {
            return OrthographicCameraDepthNdcToView(ndcDepth);
        }
        case PERSPECTIVE_CAMERA:
        {
            return PerspectiveCameraDepthNdcToView(ndcDepth);
        }
    }
    return 0;
}

///NDC Z -> NDC linear Z
float OrthographicCameraDepthNdcToLinear(float ndcDepth)
{
    return ndcDepth;
}

float PerspectiveCameraDepthNdcToLinear(float ndcDepth)
{
    return cameraData.nearFlat * ndcDepth / (ndcDepth * (cameraData.nearFlat - cameraData.farFlat) + cameraData.farFlat);
}

float DepthNdcToLinear(float ndcDepth)
{
    switch(cameraData.type)
    {
        case ORTHOGRAPHIC_CAMERA:
        {
            return OrthographicCameraDepthNdcToLinear(ndcDepth);
        }
        case PERSPECTIVE_CAMERA:
        {
            return PerspectiveCameraDepthNdcToLinear(ndcDepth);
        }
    }
    return 0;
}

///NDC Position -> View space Position
vec3 OrthographicCameraPositionNdcToView(vec3 ndcPosition)
{
    vec3 nearFlatPosition = vec3(ndcPosition.xy * cameraData.parameter.yz, -cameraData.nearFlat);
    vec3 farFlatPosition = vec3(ndcPosition.xy * cameraData.parameter.yz, -cameraData.farFlat);
    float linearDepth = ndcPosition.z;
    return nearFlatPosition * (1 - linearDepth) + farFlatPosition * linearDepth;
}

vec3 PerspectiveCameraPositionNdcToView(vec3 ndcPosition)
{
    vec3 nearFlatPosition = vec3(ndcPosition.xy * cameraData.parameter.yz, -cameraData.nearFlat);
    vec3 farFlatPosition = vec3(ndcPosition.xy * cameraData.parameter.yz * cameraData.farFlat / cameraData.nearFlat, -cameraData.farFlat);
    float linearDepth = cameraData.nearFlat * ndcPosition.z / (ndcPosition.z * (cameraData.nearFlat - cameraData.farFlat) + cameraData.farFlat);
    return nearFlatPosition * (1 - linearDepth) + farFlatPosition * linearDepth;
}
vec3 PositionNdcToView(vec3 ndcPosition)
{
    switch(cameraData.type)
    {
        case ORTHOGRAPHIC_CAMERA:
        {
            return OrthographicCameraPositionNdcToView(ndcPosition);
        }
        case PERSPECTIVE_CAMERA:
        {
            return PerspectiveCameraPositionNdcToView(ndcPosition);
        }
    }
    return vec3(0, 0, 0);
}

vec3 PositionScreenToNearFlatWorld(vec2 screenPosition)
{
    float x = cameraData.parameter.y * (2 * screenPosition.x - 1);
    float y = cameraData.parameter.z * (1 - 2 * screenPosition.y);
    vec3 up = cross(cameraData.right, cameraData.forward);
    return cameraData.position + normalize(cameraData.forward) * cameraData.nearFlat + normalize(cameraData.right) * x + normalize(-up) * y;
}

vec2 PositionScreenToNdc(vec2 imagePosition)
{
    float x = (2 * imagePosition.x  - 1);
    float y = (1 - 2 * imagePosition.y);
    return clamp(vec2(x, y), -1, 1);
}

vec3 OrthographicCameraWorldView()
{
    return normalize(cameraData.forward);
}

vec3 PerspectiveCameraWorldView(vec3 worldPosition)
{
    return normalize(worldPosition - cameraData.position);
}

vec3 CameraWorldView(vec3 worldPosition)
{
    switch(cameraData.type)
    {
        case ORTHOGRAPHIC_CAMERA:
        {
            return OrthographicCameraWorldView();
        }
        case PERSPECTIVE_CAMERA:
        {
            return PerspectiveCameraWorldView(worldPosition);
        }
    }
    return vec3(0, 0, -1);
}

vec3 CameraWorldViewToPosition(vec3 worldPosition)
{
    return normalize(worldPosition - cameraData.position);
}

#endif