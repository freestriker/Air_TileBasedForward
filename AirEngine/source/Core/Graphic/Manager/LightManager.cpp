#include "Core/Graphic/Manager/LightManager.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Asset/TextureCube.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Light/LightBase.h"
#include <map>
#include "Light/AmbientLight.h"

AirEngine::Core::Graphic::Manager::LightManager::LightManager()
	: _stagingBuffer(nullptr)
	, _forwardLightInfosBuffer(nullptr)
	, _tileBasedForwardLightInfosBuffer(nullptr)
	, _ambientTextureCube(nullptr)
	, _ambientLightInfo()
	, _mainLightInfo()
	, _ortherLightInfos()
	, _ortherLightCount()
	, _ortherLightBoundingBoxInfos()
{
	_stagingBuffer = new Instance::Buffer(sizeof(StagingLightInfos), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	_forwardLightInfosBuffer = new Instance::Buffer(sizeof(ForwardLightInfos), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	_tileBasedForwardLightInfosBuffer = new Instance::Buffer(sizeof(TileBasedForwardLightInfos), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	_tileBasedForwardLightBoundingBoxInfosBuffer = new Instance::Buffer(sizeof(TileBasedForwardLightBoundingBoxInfos), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
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
}

void AirEngine::Core::Graphic::Manager::LightManager::CopyLightInfo(Command::CommandBuffer* commandBuffer)
{
	_stagingBuffer->WriteData(
		[this](void* pointer) -> void
		{
			const VkDeviceSize dataSize = sizeof(LightInfo);
			auto offset = reinterpret_cast<char*>(pointer);
			int ortherLightCount = _ortherLightCount;
			int importantLightCount = std::min(ortherLightCount, MAX_FORWARD_ORTHER_LIGHT_COUNT);
			int unimportantLightCount = std::min(ortherLightCount - importantLightCount, MAX_FORWARD_ORTHER_LIGHT_COUNT);
			
			memcpy(offset + offsetof(StagingLightInfos, ortherLightCount), &ortherLightCount, sizeof(int));
			memcpy(offset + offsetof(StagingLightInfos, importantLightCount), &importantLightCount, sizeof(int));
			memcpy(offset + offsetof(StagingLightInfos, unimportantLightCount), &unimportantLightCount, sizeof(int));
			memcpy(offset + offsetof(StagingLightInfos, ambientLightInfo), &_ambientLightInfo, dataSize);
			memcpy(offset + offsetof(StagingLightInfos, mainLightInfo), &_mainLightInfo, dataSize);
			memcpy(offset + offsetof(StagingLightInfos, ortherLightInfos), _ortherLightInfos.data(), dataSize * ortherLightCount);
			memcpy(offset + offsetof(StagingLightInfos, ortherLightBoundingBoxInfos), _ortherLightBoundingBoxInfos.data(), sizeof(LightBoundingBox) * ortherLightCount);
		}
	);

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//Forward
	commandBuffer->CopyBuffer(_stagingBuffer, offsetof(StagingLightInfos, importantLightCount), _forwardLightInfosBuffer, offsetof(ForwardLightInfos, importantLightCount), sizeof(int));
	commandBuffer->CopyBuffer(_stagingBuffer, offsetof(StagingLightInfos, unimportantLightCount), _forwardLightInfosBuffer, offsetof(ForwardLightInfos, unimportantLightCount), sizeof(int));
	commandBuffer->CopyBuffer(_stagingBuffer, offsetof(StagingLightInfos, ambientLightInfo), _forwardLightInfosBuffer, offsetof(ForwardLightInfos, ambientLightInfo), sizeof(LightInfo));
	commandBuffer->CopyBuffer(_stagingBuffer, offsetof(StagingLightInfos, mainLightInfo), _forwardLightInfosBuffer, offsetof(ForwardLightInfos, mainLightInfo), sizeof(LightInfo));
	commandBuffer->CopyBuffer(_stagingBuffer, offsetof(StagingLightInfos, ortherLightInfos), _forwardLightInfosBuffer, offsetof(ForwardLightInfos, importantLightInfos), sizeof(LightInfo) * MAX_FORWARD_ORTHER_LIGHT_COUNT);
	commandBuffer->CopyBuffer(_stagingBuffer, offsetof(StagingLightInfos, ortherLightInfos) + MAX_FORWARD_ORTHER_LIGHT_COUNT * sizeof(LightInfo), _forwardLightInfosBuffer, offsetof(ForwardLightInfos, unimportantLightInfos), sizeof(LightInfo) * MAX_FORWARD_ORTHER_LIGHT_COUNT);

	//Tile based forward
	commandBuffer->CopyBuffer(_stagingBuffer, offsetof(StagingLightInfos, ambientLightInfo), _tileBasedForwardLightInfosBuffer, offsetof(TileBasedForwardLightInfos, ambientLightInfo), sizeof(LightInfo));
	commandBuffer->CopyBuffer(_stagingBuffer, offsetof(StagingLightInfos, mainLightInfo), _tileBasedForwardLightInfosBuffer, offsetof(TileBasedForwardLightInfos, mainLightInfo), sizeof(LightInfo));
	commandBuffer->CopyBuffer(_stagingBuffer, offsetof(StagingLightInfos, ortherLightCount), _tileBasedForwardLightInfosBuffer, offsetof(TileBasedForwardLightInfos, ortherLightCount), sizeof(int));
	commandBuffer->CopyBuffer(_stagingBuffer, offsetof(StagingLightInfos, ortherLightInfos), _tileBasedForwardLightInfosBuffer, offsetof(TileBasedForwardLightInfos, ortherLightInfos), sizeof(LightInfo) * _ortherLightCount);
	
	commandBuffer->CopyBuffer(_stagingBuffer, offsetof(StagingLightInfos, ortherLightCount), _tileBasedForwardLightBoundingBoxInfosBuffer, offsetof(TileBasedForwardLightBoundingBoxInfos, lightCount), sizeof(int));
	commandBuffer->CopyBuffer(_stagingBuffer, offsetof(StagingLightInfos, ortherLightBoundingBoxInfos), _tileBasedForwardLightBoundingBoxInfosBuffer, offsetof(TileBasedForwardLightBoundingBoxInfos, lightBoundingBoxInfos), sizeof(LightBoundingBox) * _ortherLightCount);

	commandBuffer->EndRecord();
	commandBuffer->Submit();
	commandBuffer->WaitForFinish();
}

AirEngine::Asset::TextureCube* AirEngine::Core::Graphic::Manager::LightManager::AmbientTextureCube()
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
