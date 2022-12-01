#pragma once
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"
#include <glm/vec2.hpp>
#include <array>

namespace AirEngine
{
	namespace Asset
	{
		class Mesh;
	}
	namespace Core
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Image;
				class ImageSampler;
			}
			namespace Rendering
			{
				class Material;
				class Shader;
				class FrameBuffer;
			}
		}
	}
	namespace Rendering
	{
		namespace RenderFeature
		{
			class GenerateIrradianceMap_RenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class GenerateIrradianceMap_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(GenerateIrradianceMap_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};

				class GenerateIrradianceMap_RenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
					friend class GenerateIrradianceMap_RenderFeature;
				private:
					std::array< Core::Graphic::Rendering::FrameBuffer*, 6> _frameBuffers;
					Core::Graphic::Rendering::Shader* _generateShader;
					Core::Graphic::Rendering::Material* _generateMaterial;
					Core::Graphic::Instance::Image* _targetCubeImage;
					Core::Graphic::Instance::ImageSampler* _environmentImageSampler;
					Core::Graphic::Instance::Image* _environmentImage;
					Asset::Mesh* _boxMesh;
					uint32_t _sliceIndex;
				public:
					VkExtent2D resolution;
					uint32_t stepCount;
					uint32_t sliceCount;
					std::string environmentImagePath;

					CONSTRUCTOR(GenerateIrradianceMap_RenderFeatureData)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};

				CONSTRUCTOR(GenerateIrradianceMap_RenderFeature)

			private:
				Core::Graphic::Rendering::RenderPassBase* _renderPass;

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