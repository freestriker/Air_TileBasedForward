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
#include "Core/Graphic/RenderPass/BackgroundRenderPass.h"
#include "Core/Graphic/RenderPass/OpaqueRenderPass.h"
#include "Core/Graphic/RenderPass/TransparentRenderPass.h"
#include "Core/Graphic/Shader.h"
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
#include "Core/Graphic/Material.h"
#include "Core/Graphic/CoreObject/Window.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Core/Graphic/RenderPass/TBFOpaqueRenderPass.h"

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
	qDebug() << "AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::Init()";
}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnStart()
{
	Instance::Init();
	qDebug() << "AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnStart()";
}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnThreadStart()
{
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(ShaderSlotType::UNIFORM_BUFFER, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(ShaderSlotType::STORAGE_BUFFER, { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(ShaderSlotType::UNIFORM_TEXEL_BUFFER, { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(ShaderSlotType::STORAGE_TEXEL_BUFFER, { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(ShaderSlotType::TEXTURE_CUBE, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(ShaderSlotType::TEXTURE2D, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(ShaderSlotType::TEXTURE2D_WITH_INFO, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(ShaderSlotType::STORAGE_TEXTURE2D, { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(ShaderSlotType::STORAGE_TEXTURE2D_WITH_INFO, { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }, 10);

}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnRun()
{
	AirRenderer::Utils::IntersectionChecker intersectionChecker = AirRenderer::Utils::IntersectionChecker();
	
	while (!_stopped)
	{
		Instance::StartPresentCondition().Wait();
		Utils::Log::Message("Instance::StartPresentCondition().Wait()");
		Logic::CoreObject::Instance::SetNeedIterateRenderer(true);
		Instance::StartRenderCondition().Wait();
		Utils::Log::Message("Instance::StartRenderCondition().Wait()");


		//Render
		Utils::Log::Message("Render()");

		//Lights
		auto lightCopyTask = AddTask(
			[](Command::CommandPool* graphicCommandPool, Command::CommandPool* computeCommandPool)->void 
			{
				CoreObject::Instance::LightManager().SetLightInfo(CoreObject::Instance::_lights);
				auto commandBuffer = graphicCommandPool->CreateCommandBuffer("LightCopyCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
				CoreObject::Instance::LightManager().CopyLightInfo(commandBuffer);
				graphicCommandPool->DestoryCommandBuffer(commandBuffer);
			}
		);
		lightCopyTask.wait();

		//Camera
		for (auto& component : Instance::_cameras)
		{
			auto camera = dynamic_cast<Camera::CameraBase*>(component);
			if(!(camera)) continue;
			camera->RefreshCameraInfo();

			auto viewMatrix = camera->ViewMatrix();

			//Classify renderers
			std::map<std::string, std::multimap<float, Renderer::Renderer*>> rendererDistenceMaps = std::map<std::string, std::multimap<float, Renderer::Renderer*>>();

			for (auto& rendererComponent : Instance::_renderers)
			{
				auto renderer = dynamic_cast<Renderer::Renderer*>(rendererComponent);
				if (!(renderer && renderer->GetMAterials()->size() && renderer->mesh)) continue;
				renderer->RefreshObjectInfo();

				auto obbMvCenter = viewMatrix * renderer->GameObject()->transform.ModelMatrix() * glm::vec4(renderer->mesh->OrientedBoundingBox().Center(), 1.0f);

				for (const auto& materialPair : *renderer->GetMAterials())
				{
					rendererDistenceMaps[materialPair.first].insert({ obbMvCenter.z, renderer });
				}
			}

			std::map<std::string, std::future<void>> renderTasks = std::map<std::string, std::future<void>>();
			//Add build command buffer task
			for (const auto& renderPass : *camera->_renderPassTarget->RenderPasses())
			{
				auto rendererDistanceMap = &rendererDistenceMaps[renderPass->Name()];
				auto renderPassTarget = camera->_renderPassTarget;

				renderTasks[renderPass->Name()] = AddTask(
					[renderPass, rendererDistanceMap, renderPassTarget, camera](Command::CommandPool* graphicCommandPool, Command::CommandPool* computeCommandPool)
					{
						renderPass->OnPopulateCommandBuffer(graphicCommandPool, *rendererDistanceMap, camera);
					}
				);
			}

			std::this_thread::yield();

			//Submit command buffers
			for (const auto& renderPass : *camera->_renderPassTarget->RenderPasses())
			{
				renderTasks[renderPass->Name()].wait();
				renderPass->OnSubmit();
			}

			//Clear command buffers
			for (const auto& renderPass : *camera->_renderPassTarget->RenderPasses())
			{
				renderPass->OnClear();
			}

			//Clear
			ClearCommandPools();
		}


		Logic::CoreObject::Instance::SetNeedIterateRenderer(false);
		Utils::Log::Message("Instance::EndRenderCondition().Awake()");
		Instance::EndRenderCondition().Awake();

		//Copy
		auto mainCamera = Camera::CameraBase::mainCamera;
		//auto qfi1 = Window::VulkanWindow_()->graphicsQueueFamilyIndex();
		//auto qfi2 = Instance::Queue_("GraphicQueue")->queueFamilyIndex;
		if (mainCamera)
		{
			Utils::Log::Message("Copy()");
			auto curSwapchaineImageIndex = Window::VulkanWindow_()->currentSwapChainImageIndex();
			auto curSwapchaineImage = Window::VulkanWindow_()->swapChainImage(curSwapchaineImageIndex);
			auto windowExtent = Window::Extent();

			auto copyTask = AddTask(
				[curSwapchaineImage, windowExtent, mainCamera](Command::CommandPool* graphicCommandPool, Command::CommandPool* computeCommandPool)
				{
					auto presentCommandBuffer = graphicCommandPool->CreateCommandBuffer("PresentCopyCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
					presentCommandBuffer->Reset();
					presentCommandBuffer->BeginRecord(VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
					//Present queue attachment to transfer layout
					{
						Command::ImageMemoryBarrier attachmentReleaseBarrier = Command::ImageMemoryBarrier
						(
							mainCamera->attachments["ColorAttachment"],
							VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
							VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
							VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
							VK_ACCESS_TRANSFER_READ_BIT
						);

						presentCommandBuffer->AddPipelineImageBarrier(
							VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
							{ &attachmentReleaseBarrier }
						);
					}
					//Present queue swapchain image to transfer layout
					{
						VkImageSubresourceRange subresourceRange = {};
						subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
						subresourceRange.baseMipLevel = 0;
						subresourceRange.levelCount = 1;
						subresourceRange.baseArrayLayer = 0;
						subresourceRange.layerCount = 1;

						Command::ImageMemoryBarrier transferDstBarrier = Command::ImageMemoryBarrier
						(
							curSwapchaineImage,
							VK_IMAGE_LAYOUT_UNDEFINED,
							VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							0,
							VK_ACCESS_TRANSFER_WRITE_BIT,
							{ subresourceRange }
						);

						presentCommandBuffer->AddPipelineImageBarrier(
							VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT,
							{ &transferDstBarrier }
						);
					}
					//Copy attachment
					{
						presentCommandBuffer->Blit(
							mainCamera->attachments["ColorAttachment"],
							VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
							curSwapchaineImage,
							VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							{ 0, static_cast<int>(windowExtent.height), 0 },
							VkOffset3D{ static_cast<int>(windowExtent.width), 0, 1 },
							VkFilter::VK_FILTER_LINEAR
						);
					}
					//Present queue swapchain image to present layout
					{
						VkImageSubresourceRange subresourceRange = {};
						subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
						subresourceRange.baseMipLevel = 0;
						subresourceRange.levelCount = 1;
						subresourceRange.baseArrayLayer = 0;
						subresourceRange.layerCount = 1;

						Command::ImageMemoryBarrier presentSrcBarrier = Command::ImageMemoryBarrier
						(
							curSwapchaineImage,
							VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
							VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
							VK_ACCESS_TRANSFER_WRITE_BIT,
							0,
							{ subresourceRange }
						);

						presentCommandBuffer->AddPipelineImageBarrier(
							VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TRANSFER_BIT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
							{ &presentSrcBarrier }
						);
					}
					presentCommandBuffer->EndRecord();
					presentCommandBuffer->Submit();
					presentCommandBuffer->WaitForFinish();

					graphicCommandPool->DestoryCommandBuffer(presentCommandBuffer);
				}
			);

			std::this_thread::yield();
			copyTask.wait();
		}

		Utils::Log::Message("Instance::EndPresentCondition().Awake()");
		Instance::EndPresentCondition().Awake();

		Instance::MemoryManager().Collect();
		Instance::DescriptorSetManager().Collect();

		
	}
	Instance::MemoryManager().Collect();
	Instance::DescriptorSetManager().Collect();
}

void AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnEnd()
{
	qDebug() << "AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnEnd()";
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
