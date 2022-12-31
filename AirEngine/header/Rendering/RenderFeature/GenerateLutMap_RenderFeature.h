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
			class GenerateLutMap_RenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class GenerateLutMap_Accumulation_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(GenerateLutMap_Accumulation_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};

				class GenerateLutMap_Pack_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(GenerateLutMap_Pack_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};

				class GenerateLutMap_Unpack_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(GenerateLutMap_Unpack_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};

				class GenerateLutMap_RenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
					friend class GenerateLutMap_RenderFeature;
				private:
					Core::Graphic::Rendering::FrameBuffer* _accumulationFrameBuffer;
					Core::Graphic::Rendering::FrameBuffer* _packFrameBuffer;
					Core::Graphic::Rendering::FrameBuffer* _unpackFrameBuffer;
					Core::Graphic::Rendering::Shader* _accumulationShader;
					Core::Graphic::Rendering::Material* _accumulationMaterial;
					Core::Graphic::Rendering::Shader* _packShader;
					Core::Graphic::Rendering::Material* _packMaterial;
					Core::Graphic::Rendering::Shader* _unpackShader;
					Core::Graphic::Rendering::Material* _unpackMaterial;
					Core::Graphic::Instance::Image* _accumulationImage;
					Core::Graphic::Instance::Image* _packImage;
					Core::Graphic::Instance::Image* _unpackSourceImage;
					Core::Graphic::Instance::Image* _unpackImage;
					Core::Graphic::Instance::ImageSampler* _sampler;
					Asset::Mesh* _planeMesh;
					uint32_t _sliceIndex;
				public:
					VkExtent2D resolution;
					uint32_t stepCount;
					uint32_t sliceCount;

					CONSTRUCTOR(GenerateLutMap_RenderFeatureData)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};

				CONSTRUCTOR(GenerateLutMap_RenderFeature)

			public:
				struct AccumulationPushConstantInfo
				{
					alignas(4) uint32_t stepCount;
					alignas(4) uint32_t sliceCount;
					alignas(4) uint32_t sliceIndex;
					alignas(4) uint32_t resolution;
				};
				struct PackPushConstantInfo
				{
					alignas(4) uint32_t stepCount;
					alignas(4) uint32_t resolution;
					alignas(4) uint32_t placeholder0;
					alignas(4) uint32_t placeholder1;
				};

			private:
				Core::Graphic::Rendering::RenderPassBase* _accumulationRenderPass;
				Core::Graphic::Rendering::RenderPassBase* _packRenderPass;
				Core::Graphic::Rendering::RenderPassBase* _unpackRenderPass;

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