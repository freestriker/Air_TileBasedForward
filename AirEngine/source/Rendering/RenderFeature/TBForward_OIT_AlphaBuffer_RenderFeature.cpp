#include "Rendering/RenderFeature/TBForward_OIT_AlphaBuffer_RenderFeature.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/Rendering/FrameBuffer.h"
#include "Camera/CameraBase.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Asset/Mesh.h"
#include "Renderer/Renderer.h"
#include "Utils/OrientedBoundingBox.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/Object/Transform.h"
#include "Core/Graphic/Rendering/Material.h"
#include "Core/Graphic/Manager/LightManager.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Graphic/Command/BufferMemoryBarrier.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/Rendering/Shader.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBuffer_RenderPass>("AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBuffer_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBufferBlend_RenderPass>("AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBufferBlend_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeatureData>("AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature>("AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBuffer_RenderPass::TBForward_OIT_AlphaBuffer_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBuffer_RenderPass::~TBForward_OIT_AlphaBuffer_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBuffer_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
{
	settings.AddDepthAttachment(
		"DepthAttachment",
		VK_FORMAT_D32_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	);
	settings.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ },
		"DepthAttachment"
	);
	settings.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
		VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
	);
	settings.AddDependency(
		"DrawSubpass",
		"VK_SUBPASS_EXTERNAL",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		VkAccessFlagBits::VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
		0
	);
}

AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBufferBlend_RenderPass::TBForward_OIT_AlphaBufferBlend_RenderPass()
	: RenderPassBase()
{

}

AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBufferBlend_RenderPass::~TBForward_OIT_AlphaBufferBlend_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBufferBlend_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
{
	settings.AddColorAttachment(
		"ColorAttachment",
		VK_FORMAT_R8G8B8A8_SRGB,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);
	settings.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "ColorAttachment" }
	);
	settings.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
	settings.AddDependency(
		"DrawSubpass",
		"VK_SUBPASS_EXTERNAL",
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
}

AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeatureData::TBForward_OIT_AlphaBuffer_RenderFeatureData()
	: RenderFeatureDataBase()
	, frameBuffer()
	, headIndexImage()
	, colorListBuffer()
	, depthListBuffer(nullptr)
	, indexListBuffer(nullptr)
	, atomicCounterBuffer(nullptr)
	, blendMaterial(nullptr)
	, blendFrameBuffer(nullptr)
	, attachmentSizeInfoBuffer(nullptr)
	, transparentLightIndexListsBuffer(nullptr)
{

}

AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeatureData::~TBForward_OIT_AlphaBuffer_RenderFeatureData()
{

}


AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<TBForward_OIT_AlphaBuffer_RenderPass>())
	, _renderPassName(rttr::type::get<TBForward_OIT_AlphaBuffer_RenderPass>().get_name().to_string())
	, _blendRenderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<TBForward_OIT_AlphaBufferBlend_RenderPass>())
	, _blendShader(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\TBF_OIT_AlphaBufferBlend_Shader.shader"))
	, _fullScreenMesh(Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply"))
{

}

AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::~TBForward_OIT_AlphaBuffer_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<TBForward_OIT_AlphaBuffer_RenderPass>();
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<TBForward_OIT_AlphaBufferBlend_RenderPass>();
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\TBF_OIT_AlphaBufferBlend_Shader.shader");
	Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Mesh\\BackgroundMesh.ply");
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	auto featureData = new TBForward_OIT_AlphaBuffer_RenderFeatureData();

	return featureData;
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
	auto featureData = static_cast<TBForward_OIT_AlphaBuffer_RenderFeatureData*>(renderFeatureData);

	auto cameraColorImage = camera->attachments["ColorAttachment"];
	auto extent = camera->attachments["ColorAttachment"]->VkExtent2D_();
	auto pixelCount = extent.width * extent.height;

	featureData->attachmentSizeInfoBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(AttachmentSizeInfo),
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	AttachmentSizeInfo attachmentSizeInfo = { glm::vec2(extent.width, extent.height), glm::vec2(1, 1) / glm::vec2(extent.width, extent.height) };
	featureData->attachmentSizeInfoBuffer->WriteData(&attachmentSizeInfo, sizeof(AttachmentSizeInfo));

	featureData->frameBuffer = new Core::Graphic::Rendering::FrameBuffer(
		_renderPass,
		{
			{"DepthAttachment", camera->attachments["DepthAttachment"]}
		}
	);

	featureData->headIndexImage = Core::Graphic::Instance::Image::Create2DImage(
		extent,
		VkFormat::VK_FORMAT_R32_UINT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
	);
	featureData->colorListBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(glm::vec4) * pixelCount * LIST_SIZE_FACTOR,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	featureData->depthListBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(float) * pixelCount * LIST_SIZE_FACTOR,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	featureData->indexListBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(uint32_t) * pixelCount * LIST_SIZE_FACTOR,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
	featureData->atomicCounterBuffer = new Core::Graphic::Instance::Buffer(
		sizeof(uint32_t),
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);

	featureData->blendFrameBuffer = new Core::Graphic::Rendering::FrameBuffer(
		_blendRenderPass,
		{
			{"ColorAttachment", camera->attachments["ColorAttachment"]}
		}
	);
	featureData->blendMaterial = new Core::Graphic::Rendering::Material(_blendShader);
	featureData->blendMaterial->SetStorageBuffer("colorList", featureData->colorListBuffer);
	featureData->blendMaterial->SetStorageBuffer("depthList", featureData->depthListBuffer);
	featureData->blendMaterial->SetStorageBuffer("indexList", featureData->indexListBuffer);
	featureData->blendMaterial->SetSlotData("headIndexImage", { 0 }, { {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE, featureData->headIndexImage->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_GENERAL} });
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<TBForward_OIT_AlphaBuffer_RenderFeatureData*>(renderFeatureData);

	delete featureData->frameBuffer;
	delete featureData->headIndexImage;
	delete featureData->colorListBuffer;
	delete featureData->depthListBuffer;
	delete featureData->indexListBuffer;
	delete featureData->atomicCounterBuffer;

	delete featureData->blendMaterial;
	delete featureData->blendFrameBuffer;

	delete featureData->attachmentSizeInfoBuffer;

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
	auto featureData = static_cast<TBForward_OIT_AlphaBuffer_RenderFeatureData*>(renderFeatureData);

	struct TransparentRendererWrapper
	{
		Core::Graphic::Rendering::Material* material;
		Asset::Mesh* mesh;
	};
	std::multimap<float, TransparentRendererWrapper> wrappers = std::multimap<float, TransparentRendererWrapper>();

	///Sort
	{
		auto viewMatrix = camera->ViewMatrix();
		auto ambientLightTexture = Core::Graphic::CoreObject::Instance::LightManager().AmbientTextureCube();
		for (const auto& rendererComponent : *rendererComponents)
		{
			auto material = rendererComponent->GetMaterial(_renderPassName);
			if (material == nullptr) continue;
			auto mesh = rendererComponent->mesh;

			auto obbVertexes = mesh->OrientedBoundingBox().BoundryVertexes();
			auto mvMatrix = viewMatrix * rendererComponent->GameObject()->transform.ModelMatrix();
			if (rendererComponent->enableFrustumCulling && !camera->CheckInFrustum(obbVertexes, mvMatrix)) continue;

			auto center = mesh->OrientedBoundingBox().Center();
			auto vDepth = (mvMatrix * glm::vec4(center, 1)).z;

			wrappers.emplace(std::abs(vDepth), TransparentRendererWrapper{ material , mesh });

			material->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
			material->SetUniformBuffer("meshObjectInfo", rendererComponent->ObjectInfoBuffer());
			material->SetUniformBuffer("lightInfos", Core::Graphic::CoreObject::Instance::LightManager().TileBasedForwardLightInfosBuffer());
			material->SetTextureCube("ambientLightTexture", Core::Graphic::CoreObject::Instance::LightManager().AmbientTextureCube());
			material->SetStorageBuffer("transparentLightIndexLists", featureData->transparentLightIndexListsBuffer);

			material->SetSlotData("headIndexImage", { 0 }, { {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE, featureData->headIndexImage->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_GENERAL} });
			material->SetStorageBuffer("indexList", featureData->indexListBuffer);
			material->SetStorageBuffer("colorList", featureData->colorListBuffer);
			material->SetStorageBuffer("depthList", featureData->depthListBuffer);
			material->SetStorageBuffer("atomicCounter", featureData->atomicCounterBuffer);
		}
	}

	commandBuffer->Reset();
	commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	if (wrappers.size())
	{
		///Clear
		{
			///Init head index image layout
			{
				Core::Graphic::Command::ImageMemoryBarrier headIndexImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
				(
					featureData->headIndexImage,
					VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
					VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					0,
					VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
				);
				commandBuffer->AddPipelineImageBarrier(
					VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &headIndexImageBarrier }
				);
			}

			float value = 0.0f;
			commandBuffer->ClearColorImage(featureData->headIndexImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, { static_cast<uint32_t>(0) });
			commandBuffer->FillBuffer(featureData->indexListBuffer, 0);
			commandBuffer->FillBuffer(featureData->colorListBuffer, *reinterpret_cast<uint32_t*>(&value));
			commandBuffer->FillBuffer(featureData->depthListBuffer, *reinterpret_cast<uint32_t*>(&value));
			commandBuffer->FillBuffer(featureData->atomicCounterBuffer, 0);
		}

		///Wait clear ready
		{
			Core::Graphic::Command::ImageMemoryBarrier headIndexImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->headIndexImage,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			Core::Graphic::Command::BufferMemoryBarrier colorListBarrier = Core::Graphic::Command::BufferMemoryBarrier(
				featureData->colorListBuffer,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			Core::Graphic::Command::BufferMemoryBarrier depthListBarrier = Core::Graphic::Command::BufferMemoryBarrier(
				featureData->depthListBuffer,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT
			);
			Core::Graphic::Command::BufferMemoryBarrier indexListBarrier = Core::Graphic::Command::BufferMemoryBarrier(
				featureData->indexListBuffer,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			Core::Graphic::Command::BufferMemoryBarrier atomicCounterBarrier = Core::Graphic::Command::BufferMemoryBarrier(
				featureData->atomicCounterBuffer,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			commandBuffer->AddPipelineBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				{ &headIndexImageBarrier },
				{ &colorListBarrier , &depthListBarrier, &indexListBarrier , &atomicCounterBarrier }
			);
		}

		///Wait light lists buffer ready
		{
			Core::Graphic::Command::BufferMemoryBarrier transparentLightListsBarrier = Core::Graphic::Command::BufferMemoryBarrier
			(
				featureData->transparentLightIndexListsBuffer,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			commandBuffer->AddPipelineBufferBarrier(
				VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				{ &transparentLightListsBarrier }
			);
		}

		///Build alpha buffer
		{
			commandBuffer->BeginRenderPass(_renderPass, featureData->frameBuffer);

			for (const auto& wrapper : wrappers)
			{
				commandBuffer->DrawMesh(wrapper.second.mesh, wrapper.second.material);
			}

			commandBuffer->EndRenderPass();
		}

		///Wait alpha buffer ready
		{
			Core::Graphic::Command::ImageMemoryBarrier headIndexImageBarrier = Core::Graphic::Command::ImageMemoryBarrier
			(
				featureData->headIndexImage,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkImageLayout::VK_IMAGE_LAYOUT_GENERAL,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			Core::Graphic::Command::BufferMemoryBarrier colorListBarrier = Core::Graphic::Command::BufferMemoryBarrier(
				featureData->colorListBuffer,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			Core::Graphic::Command::BufferMemoryBarrier depthListBarrier = Core::Graphic::Command::BufferMemoryBarrier(
				featureData->depthListBuffer,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT | VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			Core::Graphic::Command::BufferMemoryBarrier indexListBarrier = Core::Graphic::Command::BufferMemoryBarrier(
				featureData->indexListBuffer,
				VkAccessFlagBits::VK_ACCESS_SHADER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			commandBuffer->AddPipelineBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				{ &headIndexImageBarrier },
				{ &colorListBarrier , &depthListBarrier, &indexListBarrier }
			);
		}

		///Blend to camera color attachment
		{
			commandBuffer->BeginRenderPass(_blendRenderPass, featureData->blendFrameBuffer);

			commandBuffer->DrawMesh(_fullScreenMesh, featureData->blendMaterial);

			commandBuffer->EndRenderPass();
		}
	}

	commandBuffer->EndRecord();
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->Submit();
}

void AirEngine::Rendering::RenderFeature::TBForward_OIT_AlphaBuffer_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
	commandBuffer->WaitForFinish();
}
