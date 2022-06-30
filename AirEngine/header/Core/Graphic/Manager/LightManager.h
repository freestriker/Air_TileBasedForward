#pragma once
#include <array>
#include <glm/glm.hpp>
#include <vector>

namespace AirEngine
{
	namespace Light
	{
		class LightBase;
	}
	namespace Asset
	{
		class TextureCube;
	}
	namespace Core
	{
		namespace Logic
		{
			namespace Object
			{
				class Component;
			}
		}
		namespace Graphic
		{
			namespace CoreObject
			{
				class Instance;
				class Thread;
			}
			namespace Instance
			{
				class Buffer;
			}
			namespace Command
			{
				class CommandPool;
				class CommandBuffer;
			}
			namespace Manager
			{
				class LightManager final
				{
				public:
					LightManager();
					~LightManager();
					LightManager(const LightManager&) = delete;
					LightManager& operator=(const LightManager&) = delete;
					LightManager(LightManager&&) = delete;
					LightManager& operator=(LightManager&&) = delete;
					struct LightData
					{
						alignas(4) int type;
						alignas(4) float intensity;
						alignas(4) float minRange;
						alignas(4) float maxRange;
						alignas(16) glm::vec4 extraParamter;
						alignas(16) glm::vec3 position;
						alignas(16) glm::vec3 direction;
						alignas(16) glm::vec4 color;
					};
					void SetLightData(std::vector<Logic::Object::Component*> lights);
					void CopyLightData(Command::CommandBuffer* commandBuffer);
					Asset::TextureCube* SkyBoxTexture();
					Instance::Buffer* SkyBoxBuffer();
					Instance::Buffer* MainLightBuffer();
					Instance::Buffer* ImportantLightsBuffer();
					Instance::Buffer* UnimportantLightsBuffer();
				private:
					Asset::TextureCube* _skyBoxTexture;
					Instance::Buffer* _stageBuffer;
					Instance::Buffer* _skyBoxBuffer;
					Instance::Buffer* _mainLightBuffer;
					Instance::Buffer* _importantLightsBuffer;
					Instance::Buffer* _unimportantLightsBuffer;
					LightData _skyBoxData;
					LightData _mainLightData;
					std::array<LightData, 4> _importantLightData;
					std::array<LightData, 4> _unimportantLightData;

				};
			}
		}
	}
}
