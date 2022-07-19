#pragma once
#include "Core/Graphic/RenderPass/RenderPassBase.h"
#include "Asset/TextureCube.h"

#define MAX_LIGHT_INDEX_COUNT 64
#define TILE_WIDTH 32

namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			class Material;
			namespace Command
			{
				class CommandBuffer;
				class CommandPool;
			}
			namespace Instance
			{
				class Buffer;
				class Image;
			}
			namespace RenderPass
			{
				class TBF_OpaqueRenderPass final : public RenderPassBase
				{
				private:
					struct LightIndexList
					{
						int count;
						int indexes[MAX_LIGHT_INDEX_COUNT];
					};
					Command::CommandBuffer* _renderCommandBuffer;
					Command::CommandPool* _renderCommandPool;
					Instance::Buffer* _opauqeLightIndexListsBuffer;
					Instance::Buffer* _transparentLightIndexListsBuffer;
					Material* _buildLightListsMaterial;
					void OnPopulateRenderPassSettings(RenderPassSettings& creator)override;
					void OnPrepare(Camera::CameraBase* camera)override;
					void OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)override;
					void OnSubmit()override;
					void OnClear()override;
				public:
					TBF_OpaqueRenderPass();
					~TBF_OpaqueRenderPass();
					TBF_OpaqueRenderPass(const TBF_OpaqueRenderPass&) = delete;
					TBF_OpaqueRenderPass& operator=(const TBF_OpaqueRenderPass&) = delete;
					TBF_OpaqueRenderPass(TBF_OpaqueRenderPass&&) = delete;
					TBF_OpaqueRenderPass& operator=(TBF_OpaqueRenderPass&&) = delete;

					Instance::Buffer* TransparentLightIndexListsBuffer();
				};
			}
		}
	}
}