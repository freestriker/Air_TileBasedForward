#include "Camera/CameraBase.h"
#include "Core/Logic/Object/GameObject.h"
#include <glm/vec4.hpp>
#include "Utils/Log.h"
#include <rttr/registration>
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Asset/Mesh.h"
#include "Core/Graphic/Shader.h"
#include "Asset/TextureCube.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"

AirEngine::Camera::CameraBase* AirEngine::Camera::CameraBase::mainCamera = nullptr;

RTTR_REGISTRATION
{
	using namespace rttr;
	registration::class_<AirEngine::Camera::CameraBase>("AirEngine::Camera::Camera");
}

glm::mat4 AirEngine::Camera::CameraBase::ViewMatrix()
{
	return _cameraInfo.view;
}

glm::mat4 AirEngine::Camera::CameraBase::ProjectionMatrix()
{
	return _cameraInfo.projection;
}

const glm::vec4* AirEngine::Camera::CameraBase::ClipPlanes()
{
	OnSetClipPlanes(_cameraInfo.clipPlanes);
	return _cameraInfo.clipPlanes;
}

void AirEngine::Camera::CameraBase::RefreshCameraInfo()
{
	_cameraInfo.type = static_cast<int>(cameraType);
	_cameraInfo.nearFlat = nearFlat;
	_cameraInfo.farFlat = farFlat;
	_cameraInfo.aspectRatio = aspectRatio;
	_cameraInfo.position = GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1);
	OnSetParameter(_cameraInfo.parameter);
	OnSetSize(_cameraInfo.halfSize);
	_cameraInfo.forward = glm::normalize(glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, -1, 0)));
	_cameraInfo.right = glm::normalize(glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(1, 0, 0, 0)));
	OnSetClipPlanes(_cameraInfo.clipPlanes);
	OnSetProjectionMatrix(_cameraInfo.projection);

	glm::vec3 eye = GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, 0, 1);
	glm::vec3 center = glm::normalize(glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 0, -1, 1)));
	glm::vec3 up = glm::normalize(glm::vec3(GameObject()->transform.ModelMatrix() * glm::vec4(0, 1, 0, 0)));
	_cameraInfo.view = glm::lookAt(eye, center, up);

	_intersectionChecker.SetIntersectPlanes(_cameraInfo.clipPlanes, 6);

	_buffer->WriteData(&_cameraInfo, sizeof(CameraData));
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Camera::CameraBase::CameraInfoBuffer()
{
	return _buffer;
}

void AirEngine::Camera::CameraBase::RefreshRenderPassObject()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().DestroyRenderPassObject(_renderPassTarget);
	_renderPassTarget = Core::Graphic::CoreObject::Instance::RenderPassManager().GetRenderPassObject(renderPassNames, attachments);
}

bool AirEngine::Camera::CameraBase::CheckInFrustum(std::array<glm::vec3, 8>& vertexes, glm::mat4& matrix)
{
	return _intersectionChecker.Check(vertexes.data(), 8, matrix);
}

AirEngine::Core::Graphic::Manager::RenderPassTarget* AirEngine::Camera::CameraBase::RenderPassTarget()
{
	return _renderPassTarget;
}

AirEngine::Camera::CameraBase::CameraBase(CameraType cameraType, std::vector<std::string> renderPassNames, std::map<std::string, Core::Graphic::Instance::Image*> attachments)
	: Component(ComponentType::CAMERA)
	, attachments(attachments)
	, renderPassNames(renderPassNames)
	, _renderPassTarget(nullptr)
	, cameraType(cameraType)
	, nearFlat(3.0f)
	, farFlat(100.0f)
	, aspectRatio(16.0f / 9.0f)
	, _cameraInfo()
	, _projectionMatrix(glm::mat4(1.0f))
	, _intersectionChecker()
	, _buffer(new Core::Graphic::Instance::Buffer(sizeof(CameraData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
{
	RefreshRenderPassObject();
}

AirEngine::Camera::CameraBase::~CameraBase()
{
	delete _buffer;
}

void AirEngine::Camera::CameraBase::OnSetParameter(glm::vec4& parameter)
{
}