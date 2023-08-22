#pragma once
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include <glm/vec2.hpp>
#include <glm/ext/vector_int2.hpp>

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
			class IWave_RenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class IWave_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(IWave_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};
				struct IWaveConstantInfo
				{
					glm::ivec2 imageSize;
					int halfKernalSize;
					float deltaTime;
					float alpha;
					float sourceFactor;
					float obstructionFactor;
				};
				struct IWaveSurfaceConstantInfo
				{
					glm::ivec2 minVertexPosition;
					glm::ivec2 maxVertexPosition;
					float heightFactor;
				};
				class IWave_RenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
					friend class IWave_RenderFeature;
				private:
					IWaveConstantInfo iWaveConstantInfo;
					IWaveSurfaceConstantInfo iWaveSurfaceConstantInfo;
					bool isInitialized;
					Core::Graphic::Rendering::Shader* iWaveShader;
					Core::Graphic::Rendering::Material* material;
					Core::Graphic::Rendering::FrameBuffer* frameBuffer;
					std::vector<float> sourceTextureData;
					bool isSourceEmpty;
					std::vector<float> obstructionTextureData;
				public:
					Core::Graphic::Instance::Buffer* stagingBuffer;
					Core::Graphic::Instance::Image* sourceTexture;
					Core::Graphic::Instance::Image* obstructionTexture;
					Core::Graphic::Instance::Image* kernalImage;
					Core::Graphic::Instance::Image* heightImage;
					Core::Graphic::Instance::Image* previousHeightImage;
					Core::Graphic::Instance::Image* verticalDerivativeImage;

					CONSTRUCTOR(IWave_RenderFeatureData)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};

				CONSTRUCTOR(IWave_RenderFeature)

			private:
				Core::Graphic::Rendering::RenderPassBase* _renderPass;
				std::string _renderPassName;
				Core::Graphic::Instance::ImageSampler* _pointSampler;
				Core::Graphic::Instance::ImageSampler* _linearSampler;

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