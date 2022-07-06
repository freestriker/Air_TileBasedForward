#ifdef CAMERA_INFO_SET_INDEX

#ifndef _CAMERA_GLSL_
#define _CAMERA_GLSL_

#define ORTHOGRAPHIC_CAMERA 1
#define PERSPECTIVE_CAMERA 2

layout(set = CAMERA_INFO_SET_INDEX, binding = 0) uniform CameraInfo{
    int type;
    float nearFlat;
    float farFlat;
    float aspectRatio;
    vec3 position;
    vec2 halfSize;
    vec4 parameter;
    vec3 forward;
    vec3 right;
    vec4 clipPlanes[6];
} cameraInfo;

///NDC Z -> View space Z
float OrthographicCameraDepthN2V(float ndcDepth)
{
    return ndcDepth * (cameraInfo.nearFlat - cameraInfo.farFlat) - cameraInfo.nearFlat;
}

float PerspectiveCameraDepthN2V(float ndcDepth)
{
    return cameraInfo.nearFlat * cameraInfo.farFlat / (ndcDepth * (cameraInfo.farFlat - cameraInfo.nearFlat) - cameraInfo.farFlat);
}

float DepthN2V(float ndcDepth)
{
    switch(cameraInfo.type)
    {
        case ORTHOGRAPHIC_CAMERA:
        {
            return OrthographicCameraDepthN2V(ndcDepth);
        }
        case PERSPECTIVE_CAMERA:
        {
            return PerspectiveCameraDepthN2V(ndcDepth);
        }
    }
    return 0;
}

///NDC Z -> NDC linear Z
float OrthographicCameraDepthN2LN(float ndcDepth)
{
    return ndcDepth;
}

float PerspectiveCameraDepthN2LN(float ndcDepth)
{
    return cameraInfo.nearFlat * ndcDepth / (ndcDepth * (cameraInfo.nearFlat - cameraInfo.farFlat) + cameraInfo.farFlat);
}

float DepthN2LN(float ndcDepth)
{
    switch(cameraInfo.type)
    {
        case ORTHOGRAPHIC_CAMERA:
        {
            return OrthographicCameraDepthN2LN(ndcDepth);
        }
        case PERSPECTIVE_CAMERA:
        {
            return PerspectiveCameraDepthN2LN(ndcDepth);
        }
    }
    return 0;
}

///NDC Position -> View space Position
vec3 OrthographicCameraPositionN2V(vec3 ndcPosition)
{
    vec3 nearFlatPosition = vec3(ndcPosition.xy * cameraInfo.halfSize, -cameraInfo.nearFlat);
    vec3 farFlatPosition = vec3(ndcPosition.xy * cameraInfo.halfSize, -cameraInfo.farFlat);
    float linearDepth = ndcPosition.z;
    return nearFlatPosition * (1 - linearDepth) + farFlatPosition * linearDepth;
}

vec3 PerspectiveCameraPositionN2V(vec3 ndcPosition)
{
    vec3 nearFlatPosition = vec3(ndcPosition.xy * cameraInfo.halfSize, -cameraInfo.nearFlat);
    vec3 farFlatPosition = vec3(ndcPosition.xy * cameraInfo.halfSize * cameraInfo.farFlat / cameraInfo.nearFlat, -cameraInfo.farFlat);
    float linearDepth = cameraInfo.nearFlat * ndcPosition.z / (ndcPosition.z * (cameraInfo.nearFlat - cameraInfo.farFlat) + cameraInfo.farFlat);
    return nearFlatPosition * (1 - linearDepth) + farFlatPosition * linearDepth;
}
vec3 PositionN2V(vec3 ndcPosition)
{
    switch(cameraInfo.type)
    {
        case ORTHOGRAPHIC_CAMERA:
        {
            return OrthographicCameraPositionN2V(ndcPosition);
        }
        case PERSPECTIVE_CAMERA:
        {
            return PerspectiveCameraPositionN2V(ndcPosition);
        }
    }
    return vec3(0, 0, 0);
}

vec2 PositionS2N(vec2 imagePosition)
{
    float x = (2 * imagePosition.x  - 1);
    float y = (1 - 2 * imagePosition.y);
    return clamp(vec2(x, y), -1, 1);
}

vec3 PositionS2NFW(vec2 screenPosition)
{
    vec2 ndcPos = PositionS2N(screenPosition);
    float x = cameraInfo.halfSize.x * ndcPos.x;
    float y = cameraInfo.halfSize.y * ndcPos.y;
    vec3 up = cross(cameraInfo.right, cameraInfo.forward);
    //return cameraInfo.position + normalize(cameraInfo.forward) * cameraInfo.nearFlat + normalize(cameraInfo.right) * x + normalize(-up) * y;
    return cameraInfo.position + normalize(cameraInfo.forward) * cameraInfo.nearFlat + normalize(cameraInfo.right) * x + normalize(up) * y;
}


vec3 OrthographicCameraWViewDirection()
{
    return normalize(cameraInfo.forward);
}

vec3 PerspectiveCameraWViewDirection(vec3 worldPosition)
{
    return normalize(worldPosition - cameraInfo.position);
}

vec3 CameraWViewDirection(vec3 worldPosition)
{
    switch(cameraInfo.type)
    {
        case ORTHOGRAPHIC_CAMERA:
        {
            return OrthographicCameraWViewDirection();
        }
        case PERSPECTIVE_CAMERA:
        {
            return PerspectiveCameraWViewDirection(worldPosition);
        }
    }
    return vec3(0, 0, -1);
}

vec3 PositionViewedDirection(vec3 worldPosition)
{
    return normalize(worldPosition - cameraInfo.position);
}

#endif ///#ifndef _CAMERA_GLSL_
#endif ///#ifdef CAMERA_INFO_SET_INDEX