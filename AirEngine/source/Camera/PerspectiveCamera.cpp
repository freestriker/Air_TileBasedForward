#include "Camera/PerspectiveCamera.h"
RTTR_REGISTRATION
{
    using namespace rttr;
    registration::class_<AirEngine::Camera::PerspectiveCamera>("AirEngine::Camera::PerspectiveCamera");
}

void AirEngine::Camera::PerspectiveCamera::OnSetParameter(glm::vec4& parameter)
{
    const double pi = std::acos(-1.0);
    double halfFov = fovAngle * pi / 360.0;
    float halfHeight = std::tanf(halfFov) * nearFlat;
    float halfWidth = halfHeight * aspectRatio;
    parameter = glm::vec4(halfFov * 2, halfWidth, halfHeight, 0);
}

void AirEngine::Camera::PerspectiveCamera::OnSetClipPlanes(glm::vec4* clipPlanes)
{
    const double pi = std::acos(-1.0);
    double halfFov = fovAngle * pi / 360.0;
    float tanHalfFov = std::tan(halfFov);
    clipPlanes[0] = glm::vec4(0, -nearFlat, -nearFlat * tanHalfFov, 0);
    clipPlanes[1] = glm::vec4(0, nearFlat, -nearFlat * tanHalfFov, 0);
    clipPlanes[2] = glm::vec4(-nearFlat, 0, -aspectRatio * nearFlat * tanHalfFov, 0);
    clipPlanes[3] = glm::vec4(nearFlat, 0, -aspectRatio * nearFlat * tanHalfFov, 0);
    clipPlanes[4] = glm::vec4(0, 0, -1, -nearFlat);
    clipPlanes[5] = glm::vec4(0, 0, 1, farFlat);
}

void AirEngine::Camera::PerspectiveCamera::OnSetProjectionMatrix(glm::mat4& matrix)
{
    const double pi = std::acos(-1.0);
    double halfFov = fovAngle * pi / 360.0;
    double cot = 1.0 / std::tan(halfFov);
    float flatDistence = farFlat - nearFlat;

    matrix = glm::mat4(
        cot / aspectRatio, 0, 0, 0,
        0, cot, 0, 0,
        0, 0, -farFlat / flatDistence, -1,
        0, 0, -nearFlat * farFlat / flatDistence, 0
    );
}

AirEngine::Camera::PerspectiveCamera::PerspectiveCamera(std::vector<std::string> renderPassNames, std::map<std::string, Core::Graphic::Instance::Image*> attachments)
    : CameraBase(CameraType::PERSPECTIVE, renderPassNames, attachments)
    , fovAngle(60)
{
    nearFlat = 0.5f;
}

AirEngine::Camera::PerspectiveCamera::~PerspectiveCamera()
{
}
