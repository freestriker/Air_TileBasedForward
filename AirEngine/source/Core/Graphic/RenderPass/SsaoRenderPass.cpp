#include "Core/Graphic/RenderPass/SsaoRenderPass.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/CoreObject/Window.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/Instance/FrameBuffer.h"
#include "Renderer/Renderer.h"
#include "Core/Graphic/Material.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Camera/CameraBase.h"
#include "Utils/Log.h"
#include "Asset/Mesh.h"
#include "Utils/OrientedBoundingBox.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/Object/Transform.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Graphic/Material.h"
#include "Core/Graphic/Shader.h"
#include "Asset/Mesh.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include <random>
#include <glm/vec4.hpp>
#include "Utils/RandomSphericalCoordinateGenerator.h"
#include "Core/Graphic/Command/BufferMemoryBarrier.h"
#include <Core/Graphic/RenderPass/GeometryRenderPass.h>

void AirEngine::Core::Graphic::RenderPass::SsaoRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
{
	creator.AddColorAttachment(
		"ColorAttachment",
		VK_FORMAT_R8G8B8A8_SRGB,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);
	creator.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "ColorAttachment" }
	);
	creator.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
}

void AirEngine::Core::Graphic::RenderPass::SsaoRenderPass::OnPrepare(Camera::CameraBase* camera)
{
	auto extent = camera->RenderPassTarget()->Extent();
	_occlusionImage = Graphic::Instance::Image::Create2DImage(
		{ extent.width / 2, extent.height / 2 },
		VK_FORMAT_R32_SFLOAT,
		VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
	);
	_sizeInfoBuffer = new Instance::Buffer{
		sizeof(SizeInfo),
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	};
	_sizeInfoBuffer->WriteData(
		[this](void* ptr)->void
		{
			SizeInfo* sizeInfo = reinterpret_cast<SizeInfo*>(ptr);
			sizeInfo->attachmentSize = glm::vec2(_occlusionImage->VkExtent3D_().width, _occlusionImage->VkExtent3D_().height);
			sizeInfo->noiseTextureSize = glm::vec2(NOISE_IMAGE_WIDTH, NOISE_IMAGE_WIDTH);
			sizeInfo->scale = sizeInfo->attachmentSize / sizeInfo->noiseTextureSize;
		}
	);
	_occlusionRenderPassTarget = CoreObject::Instance::RenderPassManager().GetRenderPassObject(
		{ "SsaoOcclusionRenderPass" },
		{
			{"OcclusionAttachment", _occlusionImage}
		}
	);
	if (_occlusionMaterial == nullptr)
	{
		_occlusionMaterial = new Core::Graphic::Material(Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Shader>("..\\Asset\\Shader\\SsaoOcclusionShader.shader"));
	}
}

void AirEngine::Core::Graphic::RenderPass::SsaoRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{
	_renderCommandPool = commandPool;

	_renderCommandBuffer = commandPool->CreateCommandBuffer("SsaoCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	_renderCommandBuffer->Reset();
	_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	///Create noise texture
	if (_noiseImage == nullptr)
	{
		_noiseImage = Graphic::Instance::Image::Create2DImage(
			{ NOISE_IMAGE_WIDTH, NOISE_IMAGE_WIDTH },
			VK_FORMAT_R32_SFLOAT,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);
		auto temporaryBuffer = new Graphic::Instance::Buffer(
			sizeof(float) * NOISE_IMAGE_WIDTH * NOISE_IMAGE_WIDTH, 
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);
		{
			std::array<float, NOISE_IMAGE_WIDTH* NOISE_IMAGE_WIDTH> data = std::array<float, NOISE_IMAGE_WIDTH* NOISE_IMAGE_WIDTH>();
			std::default_random_engine engine;
			std::uniform_real_distribution<float> u(0.0f, 1.0f);
			for (auto& node : data)
			{
				node = u(engine);
			}
			temporaryBuffer->WriteData(data.data(), sizeof(float) * NOISE_IMAGE_WIDTH * NOISE_IMAGE_WIDTH);
		}

		{
			auto barrier = Command::ImageMemoryBarrier
			(
				_noiseImage,
				VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT
			);
			_renderCommandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
				{ &barrier }
			);
		}

		_renderCommandBuffer->CopyBufferToImage(temporaryBuffer, _noiseImage, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		
		{
			auto barrier = Command::ImageMemoryBarrier
			(
				_noiseImage,
				VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT,
				VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
			);
			_renderCommandBuffer->AddPipelineImageBarrier(
				VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
				{ &barrier }
			);
		}

		_renderCommandBuffer->EndRecord();
		_renderCommandBuffer->Submit();
		_renderCommandBuffer->WaitForFinish();
		_renderCommandBuffer->Reset();
		_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	}

	///Create sample kernal buffer
	if (_sampleKernalBuffer == nullptr)
	{
		_sampleKernalBuffer = new Instance::Buffer{
			sizeof(SampleKernal),
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		};
		auto stagingBuffer = new Instance::Buffer{
			sizeof(SampleKernal),
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		};
		SampleKernal sampleKernal = SampleKernal();
		sampleKernal.radius = SAMPLE_KERNAL_RADIUS;

		Utils::RandomSphericalCoordinateGenerator generator = Utils::RandomSphericalCoordinateGenerator(0, 90 - SAMPLE_BIAS_ANGLE, 0, 360, 1);
		for (int i = 0; i < SAMPLE_KERNAL_SIZE; i++)
		{
			sampleKernal.points[i] = glm::vec4(generator.Get(), std::clamp(std::pow(static_cast<float>(i + 1) / static_cast<float>(SAMPLE_KERNAL_SIZE), 2.0f), 0.1f, 1.0f));
		}
		stagingBuffer->WriteData(&sampleKernal, sizeof(SampleKernal));

		_renderCommandBuffer->CopyBuffer(stagingBuffer, _sampleKernalBuffer);

		Command::BufferMemoryBarrier barrier = Command::BufferMemoryBarrier(_sampleKernalBuffer, VkAccessFlagBits::VK_ACCESS_TRANSFER_WRITE_BIT, VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT);
		_renderCommandBuffer->AddPipelineBufferBarrier(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, { &barrier });

		_renderCommandBuffer->EndRecord();
		_renderCommandBuffer->Submit();
		_renderCommandBuffer->WaitForFinish();
		delete stagingBuffer;
		_renderCommandBuffer->Reset();
		_renderCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
	}

	//Init attachment layout
	{
		auto attachmentBarrier = Command::ImageMemoryBarrier
		(
			_occlusionImage,
			VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED,
			VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			0,
			VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
		);
		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			{ &attachmentBarrier }
		);
	}
	GeometryRenderPass& geometryRenderPass = dynamic_cast<GeometryRenderPass&>(CoreObject::Instance::RenderPassManager().RenderPass("GeometryRenderPass"));

	_occlusionMaterial->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
	_occlusionMaterial->SetSlotData("noiseTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _noiseTextureSampler->VkSampler_(), _noiseImage->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}});
	_occlusionMaterial->SetSlotData("depthTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _depthTextureSampler->VkSampler_(), geometryRenderPass.DepthImage()->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_GENERAL}});
	_occlusionMaterial->SetSlotData("normalTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, _normalTextureSampler->VkSampler_(), geometryRenderPass.NormalImage()->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}});
	_occlusionMaterial->SetUniformBuffer("sizeInfo", _sizeInfoBuffer);
	_occlusionMaterial->SetUniformBuffer("sampleKernal", _sampleKernalBuffer);


	VkClearValue occlusionAttachmentClearValue = {};
	occlusionAttachmentClearValue.color.float32[0] = 0.0f;
	_renderCommandBuffer->BeginRenderPass(_occlusionRenderPass, _occlusionRenderPassTarget, { {"OcclusionAttachment", occlusionAttachmentClearValue}});

	_renderCommandBuffer->BindMaterial(_occlusionMaterial);
	_renderCommandBuffer->DrawMesh(_fullScreenMesh);

	_renderCommandBuffer->EndRenderPass();

	///Change layout
	{
		auto attachmentBarrier = Command::ImageMemoryBarrier
		(
			_occlusionImage,
			VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			0
		);
		_renderCommandBuffer->AddPipelineImageBarrier(
			VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			{ &attachmentBarrier }
		);
	}

	_renderCommandBuffer->EndRecord();
}

void AirEngine::Core::Graphic::RenderPass::SsaoRenderPass::OnSubmit()
{
	_renderCommandBuffer->Submit();
	_renderCommandBuffer->WaitForFinish();
}

void AirEngine::Core::Graphic::RenderPass::SsaoRenderPass::OnClear()
{
	CoreObject::Instance::RenderPassManager().DestroyRenderPassObject(_occlusionRenderPassTarget);

	delete _occlusionImage;
	delete _sizeInfoBuffer;

	//delete _noiseImage;
	//_noiseImage = nullptr;

	//delete _sampleKernalBuffer;
	//_sampleKernalBuffer = nullptr;

	_renderCommandPool->DestoryCommandBuffer(_renderCommandBuffer);
}

AirEngine::Core::Graphic::RenderPass::SsaoRenderPass::SsaoRenderPass()
	: RenderPassBase("SsaoRenderPass", SSAO_RENDER_INDEX)
	, _renderCommandBuffer(nullptr)
	, _renderCommandPool(nullptr)
	, _fullScreenMesh(nullptr)
	, _occlusionMaterial(nullptr)
	, _blendMaterial(nullptr)
	, _occlusionRenderPassTarget(nullptr)
	, _occlusionImage(nullptr)
	, _noiseImage(nullptr)
	, _noiseTextureSampler(nullptr)
	, _normalTextureSampler(nullptr)
	, _depthTextureSampler(nullptr)
	, _sampleKernalBuffer(nullptr)
	, _occlusionRenderPass(nullptr)
{
	_fullScreenMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply");

	_occlusionRenderPass = new Graphic::RenderPass::SsaoOcclusionRenderPass();
	Graphic::CoreObject::Instance::RenderPassManager().AddRenderPass(_occlusionRenderPass);

	_noiseTextureSampler = new Instance::ImageSampler
	(
		VkFilter::VK_FILTER_NEAREST,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	);

	_normalTextureSampler = new Instance::ImageSampler
	(
		VkFilter::VK_FILTER_NEAREST,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	);

	_depthTextureSampler = new Instance::ImageSampler
	(
		VkFilter::VK_FILTER_NEAREST,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_WHITE
	);
}

AirEngine::Core::Graphic::RenderPass::SsaoRenderPass::~SsaoRenderPass()
{
}

void AirEngine::Core::Graphic::RenderPass::SsaoOcclusionRenderPass::OnPopulateRenderPassSettings(RenderPassSettings& creator)
{
	creator.AddColorAttachment(
		"OcclusionAttachment",
		VK_FORMAT_R32_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_CLEAR,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);
	creator.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "OcclusionAttachment"}
	);
	creator.AddDependency(
		"VK_SUBPASS_EXTERNAL",
		"DrawSubpass",
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	);
}

void AirEngine::Core::Graphic::RenderPass::SsaoOcclusionRenderPass::OnPrepare(Camera::CameraBase* camera)
{
}

void AirEngine::Core::Graphic::RenderPass::SsaoOcclusionRenderPass::OnPopulateCommandBuffer(Command::CommandPool* commandPool, std::multimap<float, Renderer::Renderer*>& renderDistanceTable, Camera::CameraBase* camera)
{
}

void AirEngine::Core::Graphic::RenderPass::SsaoOcclusionRenderPass::OnSubmit()
{
}

void AirEngine::Core::Graphic::RenderPass::SsaoOcclusionRenderPass::OnClear()
{
}

AirEngine::Core::Graphic::RenderPass::SsaoOcclusionRenderPass::SsaoOcclusionRenderPass()
	: RenderPassBase("SsaoOcclusionRenderPass", SSAO_OCCLUSION_RENDER_INDEX)
{
}

AirEngine::Core::Graphic::RenderPass::SsaoOcclusionRenderPass::~SsaoOcclusionRenderPass()
{
}
