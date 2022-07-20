#pragma once
#include "Core/Graphic/RenderPass/RenderPassBase.h"
#include "Asset/TextureCube.h"
#include <vector>
#include <array>
#include <optional>
#define ALPHA_LINKED_LIST_SIZE_FACTOR 4

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			namespace Command
			{
				class CommandBuffer;
				class CommandPool;
			}
			namespace Instance
			{
				class Buffer;
				class Image;
				class ImageSampler;
			}
			namespace Manager
			{
				class RenderPassTarget;
			}
			class Material;
			namespace RenderPass
			{
				class TBF_OIT_AlphaLinkedListRenderPass final : public RenderPassBase
				{
				private:
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;

					Instance::Image* _linkedListHeadImage;
					Instance::Buffer* _pixelColorsBuffer;
					Instance::Buffer* _pixelInfosBuffer;
					Instance::Buffer* _pixelAtomicCounterBuffer;

					std::optional<bool> _needAlphaLinkedListPass;

					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPrepare(Camera::CameraBase* camera)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					TBF_OIT_AlphaLinkedListRenderPass();
					~TBF_OIT_AlphaLinkedListRenderPass();
					TBF_OIT_AlphaLinkedListRenderPass(const TBF_OIT_AlphaLinkedListRenderPass&) = delete;
					TBF_OIT_AlphaLinkedListRenderPass& operator=(const TBF_OIT_AlphaLinkedListRenderPass&) = delete;
					TBF_OIT_AlphaLinkedListRenderPass(TBF_OIT_AlphaLinkedListRenderPass&&) = delete;
					TBF_OIT_AlphaLinkedListRenderPass& operator=(TBF_OIT_AlphaLinkedListRenderPass&&) = delete;

					Instance::Image* LinkedListHeadImage();
					Instance::Buffer* PixelColorsBuffer();
					Instance::Buffer* PixelInfosBuffer();

					bool NeedAlphaLinkedListPass();
				};
			}
		}
	}
}