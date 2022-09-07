#include "Core/Graphic/CoreObject/Thread.h"
#include "Core/Graphic/CoreObject/Window.h"
#include <QDebug>
#include <vulkan/vulkan_core.h>
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/MemoryManager.h"
#include "Core/Graphic/Manager/DescriptorSetManager.h"
#include "Utils/Log.h"
#include "Utils/Condition.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Utils/IntersectionChecker.h"
#include <future>
#include "Core/Graphic/Command/CommandPool.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/Graphic/Manager/LightManager.h"
#include "Camera/CameraBase.h"
#include "Renderer/Renderer.h"
#include "Asset/Mesh.h"
#include "Utils/OrientedBoundingBox.h"
#include "Core/Graphic/Rendering/Material.h"
#include "Core/Graphic/CoreObject/Window.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/Manager/RenderPipelineManager.h"
#include "Core/Graphic/Rendering/RenderPipelineBase.h"
#include "Core/Graphic/Rendering/RendererBase.h"
#include "Core/Graphic/Rendering/Material.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Rendering/PresentRenderPass.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Rendering/FrameBuffer.h"

AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread AirEngine::Core::Graphic::CoreObject::Thread::_graphicThread = AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread();
std::array<AirEngine::Core::Graphic::CoreObject::Thread::SubGraphicThread, 4> AirEngine::Core::Graphic::CoreObject::Thread::_subGraphicThreads = std::array<AirEngine::Core::Graphic::CoreObject::Thread::SubGraphicThread, 4>();

bool AirEngine::Core::Graphic::CoreObject::Thread::_stopped = true;
bool AirEngine::Core::Graphic::CoreObject::Thread::_canAddTask = false;

std::mutex AirEngine::Core::Graphic::CoreObject::Thread::_taskMutex = std::mutex();
std::condition_variable AirEngine::Core::Graphic::CoreObject::Thread::_taskVariable = std::condition_variable();
std::queue<std::function<void(AirEngine::Core::Graphic::Command::CommandPool*, AirEngine::Core::Graphic::Command::CommandPool*)>> AirEngine::Core::Graphic::CoreObject::Thread::_tasks = std::queue<std::function<void(AirEngine::Core::Graphic::Command::CommandPool*, AirEngine::Core::Graphic::Command::CommandPool*)>>();

AirEngine::Core::Graphic::CoreObject::Thread::Thread()
{
}

AirEngine::Core::Graphic::CoreObject::Thread::~Thread()
{
}

void AirEngine::Core::Graphic::CoreObject::Thread::ClearCommandPools()
{
	for (auto& subThread : _subGraphicThreads)
	{
		subThread.ResetCommandPool();
	}

}

void AirEngine::Core::Graphic::CoreObject::Thread::Init()
{
	_stopped = true;
	_canAddTask = false;

	_graphicThread.Init();

	for (auto& subThread : _subGraphicThreads)
	{
		subThread.Init();
	}
}

void AirEngine::Core::Graphic::CoreObject::Thread::Start()
{
	_stopped = false;
	_graphicThread.Start();
	_graphicThread.WaitForStartFinish();

	_canAddTask = true;
	for (auto& subThread : _subGraphicThreads)
	{
		subThread.Start();
		subThread.WaitForStartFinish();
	}

}

void AirEngine::Core::Graphic::CoreObject::Thread::WaitForStartFinish()
{

}

void AirEngine::Core::Graphic::CoreObject::Thread::End()
{
	_canAddTask = false;
	for (auto& subThread : _subGraphicThreads)
	{
		subThread.End();
	}

	_stopped = true;
	_graphicThread.End();
}

AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::GraphicThread()
{
}

AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::~GraphicThread()
{
}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::Init()
{

}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnStart()
{
	Instance::Init();
}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnThreadStart()
{
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(Rendering::ShaderSlotType::UNIFORM_BUFFER, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(Rendering::ShaderSlotType::STORAGE_BUFFER, { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(Rendering::ShaderSlotType::UNIFORM_TEXEL_BUFFER, { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(Rendering::ShaderSlotType::STORAGE_TEXEL_BUFFER, { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(Rendering::ShaderSlotType::TEXTURE_CUBE, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(Rendering::ShaderSlotType::TEXTURE2D, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(Rendering::ShaderSlotType::TEXTURE2D_WITH_INFO, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(Rendering::ShaderSlotType::STORAGE_TEXTURE2D, { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(Rendering::ShaderSlotType::STORAGE_TEXTURE2D_WITH_INFO, { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }, 10);

}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnRun()
{	
	while (!_stopped)
	{
		Instance::StartPresentCondition().Wait();
		Logic::CoreObject::Instance::SetNeedIterateRenderer(true);
		Instance::StartRenderCondition().Wait();

		//Lights
		auto lightCopyTask = AddTask(
			[](Command::CommandPool* graphicCommandPool, Command::CommandPool* computeCommandPool)->void 
			{
				CoreObject::Instance::LightManager().SetLightInfo(CoreObject::Instance::_lights);
				auto commandBuffer = graphicCommandPool->CreateCommandBuffer(VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
				CoreObject::Instance::LightManager().CopyLightInfo(commandBuffer);
				graphicCommandPool->DestoryCommandBuffer(commandBuffer);
			}
		);
		lightCopyTask.wait();

		std::vector<Renderer::Renderer*> rendererComponents = std::vector<Renderer::Renderer*>(Instance::_renderers.size());
		for (size_t i = 0; i < rendererComponents.size(); i++)
		{
			rendererComponents[i] = static_cast<Renderer::Renderer*>(Instance::_renderers[i]);
			rendererComponents[i]->RefreshObjectInfo();
		}

		//Camera
		for (auto& component : Instance::_cameras)
		{
			auto camera = dynamic_cast<Camera::CameraBase*>(component);
			if(!(camera)) continue;
			camera->RefreshCameraInfo();

			auto renderer = Core::Graphic::CoreObject::Instance::RenderPipelineManager().Renderer(camera->RendererName());
			auto rendererData = Core::Graphic::CoreObject::Instance::RenderPipelineManager().RendererData(camera);
			renderer->PrepareRenderer(rendererData);
		}
		for (auto& component : Instance::_cameras)
		{
			auto camera = dynamic_cast<Camera::CameraBase*>(component);
			if(!(camera)) continue;
			auto renderer = Core::Graphic::CoreObject::Instance::RenderPipelineManager().Renderer(camera->RendererName());
			auto rendererData = Core::Graphic::CoreObject::Instance::RenderPipelineManager().RendererData(camera);
			renderer->ExcuteRenderer(rendererData, camera, &rendererComponents);
		}
		for (auto& component : Instance::_cameras)
		{
			auto camera = dynamic_cast<Camera::CameraBase*>(component);
			if(!(camera)) continue;
			auto renderer = Core::Graphic::CoreObject::Instance::RenderPipelineManager().Renderer(camera->RendererName());
			auto rendererData = Core::Graphic::CoreObject::Instance::RenderPipelineManager().RendererData(camera);
			renderer->SubmitRenderer(rendererData);
		}
		for (auto& component : Instance::_cameras)
		{
			auto camera = dynamic_cast<Camera::CameraBase*>(component);
			if(!(camera)) continue;
			auto renderer = Core::Graphic::CoreObject::Instance::RenderPipelineManager().Renderer(camera->RendererName());
			auto rendererData = Core::Graphic::CoreObject::Instance::RenderPipelineManager().RendererData(camera);
			renderer->FinishRenderer(rendererData);
		}

		Logic::CoreObject::Instance::SetNeedIterateRenderer(false);
		Instance::EndRenderCondition().Awake();

		//Present
		{
			static AirEngine::Core::Graphic::Rendering::RenderPassBase* presentRenderPass = Instance::RenderPassManager().LoadRenderPass<AirEngine::Rendering::PresentRenderPass>();
			static Rendering::Shader* presentShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\PresentShader.shader");
			static Rendering::Material* presentMaterial = new Core::Graphic::Rendering::Material(presentShader);
			static Core::Graphic::Instance::ImageSampler* presentSampler = new Core::Graphic::Instance::ImageSampler
			(
				VkFilter::VK_FILTER_LINEAR,
				VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
				VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
				0.0f,
				VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
			);
			static Core::Graphic::Instance::Buffer* attachmentSizeBuffer = new Core::Graphic::Instance::Buffer(
				sizeof(glm::vec2),
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			);
			static Asset::Mesh* fullScreenMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply");

			auto mainCamera = Camera::CameraBase::mainCamera;
			if (mainCamera)
			{
				auto prp = presentRenderPass;
				auto pm = presentMaterial;
				auto ps = presentSampler;
				auto asb = attachmentSizeBuffer;
				auto extent = mainCamera->attachments["ColorAttachment"]->VkExtent2D_();
				auto fm = fullScreenMesh;
				auto curSwapchaineImageIndex = CoreObject::Window::VulkanWindow_()->currentSwapChainImageIndex();
				auto curSwapchaineImage = CoreObject::Window::VulkanWindow_()->swapChainImage(curSwapchaineImageIndex);
				auto curSwapchaineImageView = CoreObject::Window::VulkanWindow_()->swapChainImageView(curSwapchaineImageIndex);
				auto windowExtent = CoreObject::Window::Extent();

				auto ai = Graphic::Instance::Image::CreateNative2DImage(
					curSwapchaineImage, 
					curSwapchaineImageView, 
					windowExtent, 
					VK_FORMAT_B8G8R8A8_SRGB, 
					VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT, 
					VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
				);
				auto frameBuffer = new Rendering::FrameBuffer(prp, { {"SwapchainAttachment", ai} });
				glm::vec2 attachmentSize = { windowExtent.width, windowExtent.height };
				attachmentSizeBuffer->WriteData(&attachmentSize, sizeof(glm::vec2));
				pm->SetUniformBuffer("attachmentSizeInfo", attachmentSizeBuffer);
				pm->SetSlotData("colorTexture", { 0 }, { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, ps->VkSampler_(), mainCamera->attachments["ColorAttachment"]->VkImageView_(), VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} });
				AddTask(
					[mainCamera, prp, pm, ps, frameBuffer, fm](Command::CommandPool* graphicCommandPool, Command::CommandPool* computeCommandPool)
					{
						auto commandBuffer = graphicCommandPool->CreateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
						commandBuffer->Reset();
						commandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
						auto colorAttachmentBarrier = Command::ImageMemoryBarrier
						(
							mainCamera->attachments["ColorAttachment"],
							VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
							VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
							VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
							VkAccessFlagBits::VK_ACCESS_SHADER_READ_BIT
						);
						commandBuffer->AddPipelineImageBarrier(VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, { &colorAttachmentBarrier });
						VkClearValue cv = { 0, 0, 0, 255 };
						commandBuffer->BeginRenderPass(prp, frameBuffer, { {"SwapchainAttachment", cv} });
						commandBuffer->DrawMesh(fm, pm);
						commandBuffer->EndRenderPass();

						commandBuffer->EndRecord();
						commandBuffer->Submit();
						commandBuffer->WaitForFinish();
						graphicCommandPool->DestoryCommandBuffer(commandBuffer);
					}
				).wait();
			}
		}

		Instance::EndPresentCondition().Awake();

		Instance::MemoryManager().Collect();
		Instance::DescriptorSetManager().Collect();

		Instance::RenderPassManager().Collect();
	}
	Instance::MemoryManager().Collect();
	Instance::DescriptorSetManager().Collect();
	Instance::RenderPassManager().Collect();
}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnEnd()
{

}

AirEngine::Core::Graphic::CoreObject::Thread::SubGraphicThread::SubGraphicThread()
	: _graphicCommandPool(nullptr)
	, _computeCommandPool(nullptr)
	, _mutex()
{
}

AirEngine::Core::Graphic::CoreObject::Thread::SubGraphicThread::~SubGraphicThread()
{
}

void AirEngine::Core::Graphic::CoreObject::Thread::SubGraphicThread::ResetCommandPool()
{
	std::unique_lock<std::mutex> lock(_mutex);
	_graphicCommandPool->Reset();
	_computeCommandPool->Reset();
}

void AirEngine::Core::Graphic::CoreObject::Thread::SubGraphicThread::Init()
{
}

void AirEngine::Core::Graphic::CoreObject::Thread::SubGraphicThread::OnStart()
{
	_graphicCommandPool = new Core::Graphic::Command::CommandPool("GraphicQueue", VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	_computeCommandPool = new Core::Graphic::Command::CommandPool("ComputeQueue", VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
}

void AirEngine::Core::Graphic::CoreObject::Thread::SubGraphicThread::OnThreadStart()
{
}

void AirEngine::Core::Graphic::CoreObject::Thread::SubGraphicThread::OnRun()
{
	while (true)
	{
		std::function<void(Graphic::Command::CommandPool*, Graphic::Command::CommandPool*)> task;

		{
			std::unique_lock<std::mutex> lock(_taskMutex);
			_taskVariable.wait(lock, [this] { return !_canAddTask || !_tasks.empty(); });
			if (!_canAddTask && _tasks.empty())
			{
				return;
			}
			task = std::move(_tasks.front());
			_tasks.pop();
		}

		{
			std::unique_lock<std::mutex> lock(_mutex);
			task(_graphicCommandPool, _computeCommandPool);
		}
	}
}

void AirEngine::Core::Graphic::CoreObject::Thread::SubGraphicThread::OnEnd()
{
}
