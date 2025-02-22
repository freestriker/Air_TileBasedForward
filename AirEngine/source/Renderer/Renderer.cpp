#include "Renderer/Renderer.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Graphic/Rendering/Material.h"
#include <Utils/Log.h>
#include "Core/Graphic/Rendering/Shader.h"

RTTR_REGISTRATION
{
    using namespace rttr;
    registration::class_<AirEngine::Renderer::Renderer>("AirEngine::Renderer::Renderer");
}

void AirEngine::Renderer::Renderer::RefreshObjectInfo()
{
	ObjectInfo data = { _gameObject->transform.ModelMatrix(), glm::transpose(glm::inverse(_gameObject->transform.ModelMatrix())) };
	_objectInfoBuffer->WriteData(&data, sizeof(ObjectInfo));
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Renderer::Renderer::ObjectInfoBuffer()
{
	return _objectInfoBuffer;
}

void AirEngine::Renderer::Renderer::AddMaterial(Core::Graphic::Rendering::Material* material)
{
	_materials[material->Shader()->Settings()->renderPass] = material;
}

AirEngine::Core::Graphic::Rendering::Material* AirEngine::Renderer::Renderer::GetMaterial(std::string pass)
{
	auto iterator = _materials.find(pass);
	return iterator == std::end(_materials) ? nullptr : iterator->second;
}

const std::map<std::string, AirEngine::Core::Graphic::Rendering::Material*>* AirEngine::Renderer::Renderer::GetMaterials()
{
	return &_materials;
}

AirEngine::Renderer::Renderer::Renderer()
	: Component(ComponentType::RENDERER)
	, mesh(nullptr)
	, enableFrustumCulling(true)
	, _objectInfo()
	, _materials()
	, _objectInfoBuffer(new Core::Graphic::Instance::Buffer(sizeof(ObjectInfo), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT))
{
}

AirEngine::Renderer::Renderer::~Renderer()
{
}
