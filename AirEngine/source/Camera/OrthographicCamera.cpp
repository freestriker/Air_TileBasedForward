#include "Camera/OrthographicCamera.h"
RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<AirEngine::Camera::OrthographicCamera>("AirEngine::Camera::OrthographicCamera");
}

void AirEngine::Camera::OrthographicCamera::OnSetSize(glm::vec2& parameter)
{
	float halfWidth = aspectRatio * size;
	float halfHeight = size;
	parameter = glm::vec2(halfWidth, halfHeight);
}

void AirEngine::Camera::OrthographicCamera::OnSetClipPlanes(glm::vec4* clipPlanes)
{
	clipPlanes[0] = glm::vec4(-1, 0, 0, aspectRatio * size);
	clipPlanes[1] = glm::vec4(1, 0, 0, aspectRatio * size);
	clipPlanes[2] = glm::vec4(0, -1, 0, size);
	clipPlanes[3] = glm::vec4(0, 1, 0, size);
	clipPlanes[4] = glm::vec4(0, 0, -1, -nearFlat);
	clipPlanes[5] = glm::vec4(0, 0, 1, farFlat);
}

void AirEngine::Camera::OrthographicCamera::OnSetProjectionMatrix(glm::mat4& matrix)
{
	float halfWidth = aspectRatio * size;
	float halfHeight = size;
	float flatDistence = farFlat - nearFlat;

	matrix = glm::mat4(
		1.0 / halfWidth, 0, 0, 0,
		0, 1.0 / halfHeight, 0, 0,
		0, 0, -1.0 / flatDistence, 0,
		0, 0, -nearFlat / flatDistence, 1
	);
}

AirEngine::Camera::OrthographicCamera::OrthographicCamera(std::vector<std::string> renderPassNames, std::map<std::string, Core::Graphic::Instance::Image*> attachments)
	: CameraBase(CameraType::ORTHOGRAPHIC, renderPassNames, attachments)
	, size(2.25f)
{
}

AirEngine::Camera::OrthographicCamera::OrthographicCamera(std::vector<std::string> renderPassNames, std::map<std::string, Core::Graphic::Instance::Image*> attachments, std::string rendererName)
	: CameraBase(CameraType::ORTHOGRAPHIC, renderPassNames, attachments)
	, size(2.25f)
{
	InitRenderer(rendererName);
}

AirEngine::Camera::OrthographicCamera::~OrthographicCamera()
{
}
