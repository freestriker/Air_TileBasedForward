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
	qDebug() << "AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::OnThreadStart()";
	CoreObject::Instance::RenderPassManager().AddRenderPass(new RenderPass::BackgroundRenderPass());
	CoreObject::Instance::RenderPassManager().AddRenderPass(new RenderPass::OpaqueRenderPass());
	CoreObject::Instance::RenderPassManager().AddRenderPass(new RenderPass::TransparentRenderPass());

	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(ShaderSlotType::UNIFORM_BUFFER, { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(ShaderSlotType::TEXTURE_CUBE, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(ShaderSlotType::TEXTURE2D, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }, 10);
	CoreObject::Instance::DescriptorSetManager().AddDescriptorSetPool(ShaderSlotType::TEXTURE2D_WITH_INFO, { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER }, 10);

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

		std::map<std::string, std::future<Graphic::Command::CommandBuffer*>> commandBufferTaskMap = std::map<std::string, std::future<Graphic::Command::CommandBuffer*>>();
		std::map<std::string, std::multimap<float, Renderer::Renderer*>> rendererDistenceMaps = std::map<std::string, std::multimap<float, Renderer::Renderer*>>();

		//Lights
		auto lightCopyTask = AddTask(
			[](Command::CommandPool* graphicCommandPool, Command::CommandPool* computeCommandPool)->void 
			{
				CoreObject::Instance::LightManager().SetLightData(CoreObject::Instance::_lights);
				auto commandBuffer = graphicCommandPool->CreateCommandBuffer("LightCopyCommandBuffer", VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY);
				CoreObject::Instance::LightManager().CopyLightData(commandBuffer);
				graphicCommandPool->DestoryCommandBuffer(commandBuffer);
			}
		);


		Logic::CoreObject::Instance::SetNeedIterateRenderer(false);
		Utils::Log::Message("Instance::EndRenderCondition().Awake()");
		Instance::EndRenderCondition().Awake();
		//Copy
		Utils::Log::Message("Copy()");
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
