#pragma once
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"
#include <glm/glm.hpp>

#define MAX_LIGHT_INDEX_COUNT 64
#define TILE_WIDTH 32

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Rendering
			{
				class Shader;
				class Material;
			}
			namespace Instance
			{
				class ImageSampler;
			}
		}
	}
	namespace Rendering
	{
		namespace RenderFeature
		{
			class TBForward_LightList_RenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class TBForward_LightList_RenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
					friend class TBForward_LightList_RenderFeature;
				private:
					Core::Graphic::Rendering::Material* material;
					Core::Graphic::Instance::Buffer* depthTextureInfoBuffer;
				public:
					Core::Graphic::Instance::Buffer* opaqueLightIndexListsBuffer;
					Core::Graphic::Instance::Buffer* transparentLightIndexListsBuffer;
					Core::Graphic::Instance::Image* depthTexture;

					CONSTRUCTOR(TBForward_LightList_RenderFeatureData)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};

				CONSTRUCTOR(TBForward_LightList_RenderFeature)

			private:
				struct LightIndexList
				{
					int count;
					int indexes[MAX_LIGHT_INDEX_COUNT];
				};
				struct DepthTextureInfo
				{
					alignas(8) glm::ivec2 size;
					alignas(8) glm::vec2 texelSize;
				};
				Core::Graphic::Rendering::Shader* _shader;
				Core::Graphic::Instance::ImageSampler* _depthTextureSampler;

				Core::Graphic::Rendering::RenderFeatureDataBase* OnCreateRenderFeatureData(Camera::CameraBase* camera)override;
				void OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)override;
				void OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)override;

				void OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)override;
				void OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*>const* rendererComponents)override;
				void OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)override;
				void OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)override;

				RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureBase)
			};
		}
	}
}