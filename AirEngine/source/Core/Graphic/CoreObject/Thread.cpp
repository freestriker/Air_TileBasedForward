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

AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread AirEngine::Core::Graphic::CoreObject::Thread::_graphicThread = AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread();

AirEngine::Core::Graphic::CoreObject::Thread::Thread()
{
}

AirEngine::Core::Graphic::CoreObject::Thread::~Thread()
{
}

void AirEngine::Core::Graphic::CoreObject::Thread::Init()
{
	_graphicThread.Init();
}

void AirEngine::Core::Graphic::CoreObject::Thread::Start()
{
	_graphicThread.Start();
}

void AirEngine::Core::Graphic::CoreObject::Thread::End()
{
	_graphicThread.End();
}

void AirEngine::Core::Graphic::CoreObject::Thread::WaitForStartFinish()
{
	_graphicThread.WaitForStartFinish();
}

AirEngine::Core::Graphic::CoreObject::Thread::GraphicThread::GraphicThread()
	: _stopped(true)
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
	_stopped = false;
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
	while (!_stopped)
	{
		Instance::StartPresentCondition().Wait();
		Utils::Log::Message("Instance::StartPresentCondition().Wait()");
		Logic::CoreObject::Instance::SetNeedIterateRenderer(true);
		Instance::StartRenderCondition().Wait();
		Utils::Log::Message("Instance::StartRenderCondition().Wait()");
		//Render
		Utils::Log::Message("Render()");
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
