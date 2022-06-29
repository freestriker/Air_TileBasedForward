#include "Renderer/Renderer.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Graphic/Material.h"
#include <Utils/Log.h>
RTTR_REGISTRATION
{
    using namespace rttr;
    registration::class_<AirEngine::Renderer::Renderer>("AirEngine::Renderer::Renderer");
}

void AirEngine::Renderer::Renderer::SetMatrixData(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix)
{
	MatrixData data = { _gameObject->transform.ModelMatrix() , viewMatrix , projectionMatrix, glm::transpose(glm::inverse(_gameObject->transform.ModelMatrix())) };
	_matrixBuffer->WriteData(&data, sizeof(MatrixData));
	material->SetUniformBuffer("matrixData", _matrixBuffer);
}

AirEngine::Renderer::Renderer::Renderer()
	: Component(ComponentType::RENDERER)
	, _matrixBuffer(new Core::Graphic::Instance::Buffer(sizeof(MatrixData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
	, mesh(nullptr)
	, material(nullptr)
	, enableFrustumCulling(true)
{
}

AirEngine::Renderer::Renderer::~Renderer()
{
}
