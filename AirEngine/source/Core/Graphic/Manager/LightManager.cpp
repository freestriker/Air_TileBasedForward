#include "Core/Graphic/Manager/LightManager.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Asset/TextureCube.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Light/LightBase.h"
#include <map>
#include "Light/SkyBox.h"

AirEngine::Core::Graphic::Manager::LightManager::LightManager()
	: _stageBuffer(nullptr)
	, _skyBoxTexture(nullptr)
	, _skyBoxBuffer(nullptr)
	, _mainLightBuffer(nullptr)
	, _importantLightsBuffer(nullptr)
	, _unimportantLightsBuffer(nullptr)
	, _skyBoxData()
	, _mainLightData()
	, _importantLightData()
	, _unimportantLightData()
{
	VkDeviceSize dataSize = sizeof(LightData);
	_stageBuffer = new Instance::Buffer(dataSize * 10, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	_skyBoxBuffer = new Instance::Buffer(dataSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	_mainLightBuffer = new Instance::Buffer(dataSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	_importantLightsBuffer = new Instance::Buffer(dataSize * 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	_unimportantLightsBuffer = new Instance::Buffer(dataSize * 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

AirEngine::Core::Graphic::Manager::LightManager::~LightManager()
{
}

void AirEngine::Core::Graphic::Manager::LightManager::SetLightData(std::vector<Logic::Object::Component*> lights)
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
			case Light::LightBase::LightType::SKY_BOX:
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
	//skybox
	auto skyBoxIterator = unqiueLights.find(Light::LightBase::LightType::SKY_BOX);
	if (skyBoxIterator != std::end(unqiueLights))
	{
		auto data = skyBoxIterator->second->GetLightData();
		_skyBoxData = *reinterpret_cast<LightData*>(&data);
		_skyBoxTexture = static_cast<Light::SkyBox*>(skyBoxIterator->second)->skyBoxTextureCube;
	}
	else
	{
		_skyBoxData = {};
	}

	//main
	auto directionalIterator = unqiueLights.find(Light::LightBase::LightType::DIRECTIONAL);
	if (directionalIterator != std::end(unqiueLights))
	{
		auto data = directionalIterator->second->GetLightData();
		_mainLightData = *reinterpret_cast<LightData*>(&data);
	}
	else
	{
		_mainLightData = {};
	}

	auto otherLightIter = otherLights.begin();

	//important
	for (uint32_t i = 0; i < 4; i++)
	{
		if (otherLightIter != std::end(otherLights))
		{
			auto data = (*otherLightIter)->GetLightData();
			_importantLightData[i] = *reinterpret_cast<LightData*>(&data);
			otherLightIter++;
		}
		else
		{
			_importantLightData[i] = {};
		}
	}

	//unimportant
	for (uint32_t i = 0; i < 4; i++)
	{
		if (otherLightIter != std::end(otherLights))
		{
			auto data = (*otherLightIter)->GetLightData();
			_unimportantLightData[i] = *reinterpret_cast<LightData*>(&data);
			otherLightIter++;
		}
		else
		{
			_unimportantLightData[i] = {};
		}
	}
}

void AirEngine::Core::Graphic::Manager::LightManager::CopyLightData(Command::CommandBuffer* commandBuffer)
{
	_stageBuffer->WriteData([this](void* pointer) -> void {
		VkDeviceSize dataSize = sizeof(LightData);
		memcpy(pointer, &_skyBoxData, dataSize);
		memcpy(reinterpret_cast<char*>(pointer) + dataSize, &_mainLightData, dataSize);
		memcpy(reinterpret_cast<char*>(pointer) + dataSize * 2, _importantLightData.data(), dataSize * 4);
		memcpy(reinterpret_cast<char*>(pointer) + dataSize * 6, _unimportantLightData.data(), dataSize * 4);
	});

	VkDeviceSize dataSize = sizeof(LightData);
	commandBuffer->Reset();
	commandBuffer->BeginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	commandBuffer->CopyBuffer(_stageBuffer, 0, _skyBoxBuffer, 0, dataSize);
	commandBuffer->CopyBuffer(_stageBuffer, dataSize, _mainLightBuffer, 0, dataSize);
	commandBuffer->CopyBuffer(_stageBuffer, dataSize * 2, _importantLightsBuffer, 0, dataSize * 4);
	commandBuffer->CopyBuffer(_stageBuffer, dataSize * 6, _unimportantLightsBuffer, 0, dataSize * 4);
	commandBuffer->EndRecord();
	commandBuffer->Submit();
	commandBuffer->WaitForFinish();
}

AirEngine::Asset::TextureCube* AirEngine::Core::Graphic::Manager::LightManager::SkyBoxTexture()
{
	return _skyBoxTexture;
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::Manager::LightManager::SkyBoxBuffer()
{
	return _skyBoxBuffer;
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::Manager::LightManager::MainLightBuffer()
{
	return _mainLightBuffer;
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::Manager::LightManager::ImportantLightsBuffer()
{
	return _importantLightsBuffer;
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::Manager::LightManager::UnimportantLightsBuffer()
{
	return _unimportantLightsBuffer;
}
