#include "Core/Graphic/Manager/LightManager.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Light/LightBase.h"
#include <map>
#include "Light/AmbientLight.h"

AirEngine::Core::Graphic::Manager::LightManager::LightManager()
	: _forwardLightInfosBuffer(nullptr)
	, _tileBasedForwardLightInfosBuffer(nullptr)
	, _ambientTextureCube(nullptr)
	, _ambientLightInfo()
	, _mainLightInfo()
	, _ortherLightInfos()
	, _ortherLightCount()
	, _ortherLightBoundingBoxInfos()
{
	_forwardLightInfosBuffer = new Instance::Buffer(sizeof(ForwardLightInfos), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	_tileBasedForwardLightInfosBuffer = new Instance::Buffer(sizeof(TileBasedForwardLightInfos), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	_tileBasedForwardLightBoundingBoxInfosBuffer = new Instance::Buffer(sizeof(TileBasedForwardLightBoundingBoxInfos), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

AirEngine::Core::Graphic::Manager::LightManager::~LightManager()
{
}

void AirEngine::Core::Graphic::Manager::LightManager::SetLightInfo(std::vector<Logic::Object::Component*> lights)
{
	std::multimap<Light::LightBase::LightType, Light::LightBase*> unqiueLights = std::multimap<Light::LightBase::LightType, Light::LightBase*>();
	std::vector< Light::LightBase*> otherLights = std::vector< Light::LightBase*>();
	for (const auto& lightComponent : lights)
	{
		Light::LightBase* light = static_cast<Light::LightBase*>(lightComponent);
		switch (light->lightType)
		{
			case Light::LightBase::LightType::DIRECTIONAL:
			{
				unqiueLights.emplace(std::make_pair(light->lightType, light));
				_mainLight = light;
				break;
			}
			case Light::LightBase::LightType::POINT:
			{
				otherLights.emplace_back(light);
				break;
			}
			case Light::LightBase::LightType::AMBIENT:
			{
				unqiueLights.emplace(std::make_pair(light->lightType, light));
				break;
			}
			case Light::LightBase::LightType::SPOT:
			{
				otherLights.emplace_back(light);
				break;
			}
			default:
				break;
		}
	}

	//ambient
	auto skyBoxIterator = unqiueLights.find(Light::LightBase::LightType::AMBIENT);
	if (skyBoxIterator != std::end(unqiueLights))
	{
		auto data = skyBoxIterator->second->GetLightInfo();
		_ambientLightInfo = *reinterpret_cast<const LightInfo*>(data);
		_ambientTextureCube = static_cast<Light::AmbientLight*>(skyBoxIterator->second)->ambientLightTextureCube;
	}
	else
	{
		_ambientLightInfo = {};
	}

	//main
	auto directionalIterator = unqiueLights.find(Light::LightBase::LightType::DIRECTIONAL);
	if (directionalIterator != std::end(unqiueLights))
	{
		auto data = directionalIterator->second->GetLightInfo();
		_mainLightInfo = *reinterpret_cast<const LightInfo*>(data);
	}
	else
	{
		_mainLightInfo = {};
	}

	_ortherLightInfos = {};
	_ortherLightBoundingBoxInfos = {};
	_ortherLightCount = std::min(MAX_ORTHER_LIGHT_COUNT, static_cast<int>(otherLights.size()));
	for (int i = 0; i < _ortherLightCount; i++)
	{
		auto data = otherLights[i]->GetLightInfo();
		auto boxData = otherLights[i]->GetBoundingBox();
		_ortherLightInfos[i] = *reinterpret_cast<const LightInfo*>(data);
		_ortherLightBoundingBoxInfos[i] = *reinterpret_cast<const LightBoundingBox*>(boxData->data());
	}


	_forwardLightInfosBuffer->WriteData(
		[this](void* pointer) -> void
		{
			int ortherLightCount = _ortherLightCount;
			int importantLightCount = std::min(ortherLightCount, MAX_FORWARD_ORTHER_LIGHT_COUNT);
			int unimportantLightCount = std::min(ortherLightCount - importantLightCount, MAX_FORWARD_ORTHER_LIGHT_COUNT);

			auto offset = reinterpret_cast<char*>(pointer);
			memcpy(offset + offsetof(ForwardLightInfos, importantLightCount), &importantLightCount, sizeof(int));
			memcpy(offset + offsetof(ForwardLightInfos, unimportantLightCount), &unimportantLightCount, sizeof(int));
			memcpy(offset + offsetof(ForwardLightInfos, ambientLightInfo), &_ambientLightInfo, sizeof(LightInfo));
			memcpy(offset + offsetof(ForwardLightInfos, mainLightInfo), &_mainLightInfo, sizeof(LightInfo));
			memcpy(offset + offsetof(ForwardLightInfos, importantLightInfos), _ortherLightInfos.data(), sizeof(LightInfo) * importantLightCount);
			memcpy(offset + offsetof(ForwardLightInfos, unimportantLightInfos), _ortherLightInfos.data() + sizeof(LightInfo) * importantLightCount, sizeof(LightInfo) * unimportantLightCount);
		}
	);

	_tileBasedForwardLightInfosBuffer->WriteData(
		[this](void* pointer) -> void
		{
			int ortherLightCount = std::min(_ortherLightCount, MAX_TILE_BASED_FORWARD_ORTHER_LIGHT_COUNT);

			auto offset = reinterpret_cast<char*>(pointer);
			memcpy(offset + offsetof(TileBasedForwardLightInfos, ortherLightCount), &ortherLightCount, sizeof(int));
			memcpy(offset + offsetof(TileBasedForwardLightInfos, ambientLightInfo), &_ambientLightInfo, sizeof(LightInfo));
			memcpy(offset + offsetof(TileBasedForwardLightInfos, mainLightInfo), &_mainLightInfo, sizeof(LightInfo));
			memcpy(offset + offsetof(TileBasedForwardLightInfos, ortherLightInfos), _ortherLightInfos.data(), sizeof(LightInfo) * ortherLightCount);
		}
	);

	_tileBasedForwardLightBoundingBoxInfosBuffer->WriteData(
		[this](void* pointer) -> void
		{
			int ortherLightCount = std::min(_ortherLightCount, MAX_TILE_BASED_FORWARD_ORTHER_LIGHT_COUNT);

			auto offset = reinterpret_cast<char*>(pointer);
			memcpy(offset + offsetof(TileBasedForwardLightBoundingBoxInfos, lightCount), &ortherLightCount, sizeof(int));
			memcpy(offset + offsetof(TileBasedForwardLightBoundingBoxInfos, lightBoundingBoxInfos), _ortherLightBoundingBoxInfos.data(), sizeof(LightBoundingBox) * ortherLightCount);
		}
	);
}

AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Manager::LightManager::AmbientTextureCube()
{
	return _ambientTextureCube;
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::Manager::LightManager::ForwardLightInfosBuffer()
{
	return _forwardLightInfosBuffer;
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::Manager::LightManager::TileBasedForwardLightInfosBuffer()
{
	return _tileBasedForwardLightInfosBuffer;
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::Manager::LightManager::TileBasedForwardLightBoundindBoxInfosBuffer()
{
	return _tileBasedForwardLightBoundingBoxInfosBuffer;
}

AirEngine::Core::Graphic::Manager::LightManager::LightInfo AirEngine::Core::Graphic::Manager::LightManager::MainLightInfo()
{
	return _mainLightInfo;
}

AirEngine::Light::LightBase* AirEngine::Core::Graphic::Manager::LightManager::MainLight()
{
	return _mainLight;
}
