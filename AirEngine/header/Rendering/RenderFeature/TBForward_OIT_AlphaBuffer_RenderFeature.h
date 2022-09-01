#pragma once
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"
#include <glm/glm.hpp>

#define LIST_SIZE_FACTOR 4

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
			class TBForward_OIT_AlphaBuffer_RenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class TBForward_OIT_AlphaBuffer_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(TBForward_OIT_AlphaBuffer_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};
				class TBForward_OIT_AlphaBufferBlend_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(TBForward_OIT_AlphaBufferBlend_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};

				class TBForward_OIT_AlphaBuffer_RenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
					friend class TBForward_OIT_AlphaBuffer_RenderFeature;
				private:
					Core::Graphic::Rendering::FrameBuffer* frameBuffer;
					Core::Graphic::Instance::Image* headIndexImage;
					Core::Graphic::Instance::Buffer* colorListBuffer;
					Core::Graphic::Instance::Buffer* depthListBuffer;
					Core::Graphic::Instance::Buffer* indexListBuffer;
					Core::Graphic::Instance::Buffer* atomicCounterBuffer;
					Core::Graphic::Rendering::Material* blendMaterial;
					Core::Graphic::Rendering::FrameBuffer* blendFrameBuffer;
					Core::Graphic::Instance::Buffer* attachmentSizeInfoBuffer;
				public:
					Core::Graphic::Instance::Buffer* transparentLightIndexListsBuffer;

					CONSTRUCTOR(TBForward_OIT_AlphaBuffer_RenderFeatureData)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};

				CONSTRUCTOR(TBForward_OIT_AlphaBuffer_RenderFeature)

			private:
				struct AttachmentSizeInfo
				{
					alignas(8) glm::vec2 size;
					alignas(8) glm::vec2 texelSize;
				};
				Core::Graphic::Rendering::RenderPassBase* _renderPass;
				std::string _renderPassName;
				Core::Graphic::Rendering::RenderPassBase* _blendRenderPass;
				Core::Graphic::Rendering::Shader* _blendShader;
				Asset::Mesh* _fullScreenMesh;

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