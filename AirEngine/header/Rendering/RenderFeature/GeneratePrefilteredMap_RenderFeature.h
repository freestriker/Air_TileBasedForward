#pragma once
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"
#include <glm/glm.hpp>
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
			class GeneratePrefilteredMap_RenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class GeneratePrefilteredMap_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(GeneratePrefilteredMap_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};

				class GeneratePrefilteredMap_RenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
					friend class GeneratePrefilteredMap_RenderFeature;
				private:
					std::vector< Core::Graphic::Rendering::FrameBuffer*> _frameBuffers;
					Core::Graphic::Rendering::Shader* _generateShader;
					Core::Graphic::Rendering::Material* _generateMaterial;
					Core::Graphic::Instance::Buffer* _weightInfoBuffer;
					Core::Graphic::Instance::Image* _targetCubeImage;
					Core::Graphic::Instance::ImageSampler* _environmentImageSampler;
					Core::Graphic::Instance::Image* _environmentImage;
					Asset::Mesh* _boxMesh;
					uint32_t _sliceIndex;
				public:
					VkExtent2D resolution;
					uint32_t roughnessLevelCount;
					uint32_t stepCount;
					uint32_t sliceCount;
					std::string environmentImagePath;

					CONSTRUCTOR(GeneratePrefilteredMap_RenderFeatureData)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};

				CONSTRUCTOR(GeneratePrefilteredMap_RenderFeature)

			public:
				struct AddPushConstantInfo
				{
					alignas(4) uint32_t stepCount;
					alignas(4) uint32_t sliceCount;
					alignas(4) uint32_t sliceIndex;
					alignas(4) uint32_t resolution;
					alignas(4) uint32_t faceIndex;
					alignas(4) float roughness;
					alignas(16) glm::mat4 viewProjection;
				};
				struct DivisionPushConstantInfo
				{
					alignas(4) uint32_t sliceCount;
					alignas(4) uint32_t sliceIndex;
				};
				struct WeightInfo
				{
					alignas(4) float weight;
				};

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