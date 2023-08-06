#include "Test/RendererDataController.h"
#include "Audio/AudioSource.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Asset/AudioClip.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include <algorithm>
#include "Core/Logic/Manager/InputManager.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Asset/Mesh.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Core/Graphic/Rendering/Material.h"
#include <future>
#include "Renderer/Renderer.h"
#include "Camera/CameraBase.h"
#include "Rendering/Renderer/TBForwardRenderer.h"
#include "Rendering/Renderer/AmbientOcclusionRenderer.h"
#include "Rendering/Renderer/ScreenSpaceShadowVisualizationRenderer.h"
#include "Core/Graphic/Rendering/RenderPipelineBase.h"
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
#include "Core/Graphic/Manager/RenderPipelineManager.h"
#include <Core/Graphic/CoreObject/Instance.h>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::RendererDataController>("AirEngine::Test::RendererDataController");
}

AirEngine::Test::RendererDataController::RendererDataController()
{
}

AirEngine::Test::RendererDataController::~RendererDataController()
{
}

void AirEngine::Test::RendererDataController::OnAwake()
{
}

std::string AirEngine::Test::RendererDataController::OutputRendererDescriptions()
{
	auto camera = GameObject()->GetComponent<Camera::CameraBase>();
	auto rendererDataBase = camera->RendererData();

	Utils::Log::Message("Current renderer name: " + camera->RendererName() + ".");
	auto&& renderer = *AirEngine::Core::Graphic::CoreObject::Instance::RenderPipelineManager().RenderPipeline()->Renderer(camera->RendererName());
	Utils::Log::Message("It contains:");
	for (const auto& renderFeature : renderer.RendererFeatures())
	{
		Utils::Log::Message(renderFeature.first + ", " + renderFeature.second->Description());
	}

	return "";
}

void AirEngine::Test::RendererDataController::OnStart()
{
	auto&& renderPipeline = *AirEngine::Core::Graphic::CoreObject::Instance::RenderPipelineManager().RenderPipeline();
	Utils::Log::Message("---------------------------------------------------------------");
	Utils::Log::Message(renderPipeline.Description());
	std::string names = "";
	for (const auto& renderer : renderPipeline.Renderers())
	{
		names += renderer.first + " ";
	}
	Utils::Log::Message("It contains: " + names + ".");
	OutputRendererDescriptions();
	Utils::Log::Message("Use 'L' to change renderers, use 'J' to change render features.");
	Utils::Log::Message("---------------------------------------------------------------");
}

void AirEngine::Test::RendererDataController::OnUpdate()
{
	auto camera = GameObject()->GetComponent<Camera::CameraBase>();
	auto rendererDataBase = camera->RendererData();

	///AO type
	{
		auto rendererData = dynamic_cast<Rendering::Renderer::AmbientOcclusionRenderer::AmbientOcclusionRendererData*>(rendererDataBase);
		if (rendererData != nullptr && Core::Logic::CoreObject::Instance::InputManager().KeyUp(Core::Logic::Manager::InputKeyType::Key_J))
		{
			if (rendererData->aoType == Rendering::Renderer::AmbientOcclusionRenderer::AoType::SSAO)
			{
				rendererData->aoType = Rendering::Renderer::AmbientOcclusionRenderer::AoType::HBAO;
				Utils::Log::Message("Switch AO mode to HBAO.");
			}
			else if (rendererData->aoType == Rendering::Renderer::AmbientOcclusionRenderer::AoType::HBAO)
			{
				rendererData->aoType = Rendering::Renderer::AmbientOcclusionRenderer::AoType::GTAO;
				Utils::Log::Message("Switch AO mode to GTAO.");
			}
			else if (rendererData->aoType == Rendering::Renderer::AmbientOcclusionRenderer::AoType::GTAO)
			{
				rendererData->aoType = Rendering::Renderer::AmbientOcclusionRenderer::AoType::SSAO;
				Utils::Log::Message("Switch AO mode to SSAO.");
			}
		}
	}

	///transparent type
	{
		auto rendererData = dynamic_cast<Rendering::Renderer::TBForwardRenderer::TBForwardRendererData*>(rendererDataBase);
		if (rendererData != nullptr && Core::Logic::CoreObject::Instance::InputManager().KeyUp(Core::Logic::Manager::InputKeyType::Key_J))
		{
			if (rendererData->transparentType == Rendering::Renderer::TBForwardRenderer::TransparentType::DEPTH_PEELING)
			{
				rendererData->transparentType = Rendering::Renderer::TBForwardRenderer::TransparentType::ALPHA_BUFFER;
				Utils::Log::Message("Switch AO mode to HBAO.");
			}
			else if (rendererData->transparentType == Rendering::Renderer::TBForwardRenderer::TransparentType::ALPHA_BUFFER)
			{
				rendererData->transparentType = Rendering::Renderer::TBForwardRenderer::TransparentType::DEPTH_SORT;
				Utils::Log::Message("Switch AO mode to GTAO.");
			}
			else if (rendererData->transparentType == Rendering::Renderer::TBForwardRenderer::TransparentType::DEPTH_SORT)
			{
				rendererData->transparentType = Rendering::Renderer::TBForwardRenderer::TransparentType::DEPTH_PEELING;
				Utils::Log::Message("Switch AO mode to SSAO.");
			}
		}
	}

	///Shadow type
	{
		auto rendererData = dynamic_cast<Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::ScreenSpaceShadowVisualizationRendererData*>(rendererDataBase);
		if (rendererData != nullptr)
		{
			if (Core::Logic::CoreObject::Instance::InputManager().KeyUp(Core::Logic::Manager::InputKeyType::Key_J))
			{
				if (rendererData->shadowType == Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::ShadowType::CSM)
				{
					rendererData->shadowType = Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::ShadowType::CASCADE_EVSM;
					Utils::Log::Message("Switch Shadow mode to ScreenSpaceCascadeEVSM.");
				}
				else if (rendererData->shadowType == Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::ShadowType::CASCADE_EVSM)
				{
					rendererData->shadowType = Rendering::Renderer::ScreenSpaceShadowVisualizationRenderer::ShadowType::CSM;
					Utils::Log::Message("Switch Shadow mode to ScreenSpaceCSM.");
				}
			}
		}
	}

	///Renderer type
	if (Core::Logic::CoreObject::Instance::InputManager().KeyUp(Core::Logic::Manager::InputKeyType::Key_L))
	{
		if (camera->RendererName() == "TBForwardRenderer")
		{
			camera->SetRendererName("AmbientOcclusionRenderer");
			Utils::Log::Message("---------------------------------------------------------------");
			OutputRendererDescriptions();
			Utils::Log::Message("---------------------------------------------------------------");
		}
		else if (camera->RendererName() == "AmbientOcclusionRenderer")
		{
			camera->SetRendererName("ScreenSpaceShadowVisualizationRenderer");
			Utils::Log::Message("---------------------------------------------------------------");
			OutputRendererDescriptions();
			Utils::Log::Message("---------------------------------------------------------------");
		}
		else if (camera->RendererName() == "ScreenSpaceShadowVisualizationRenderer")
		{
			camera->SetRendererName("BuildIblRenderer");
			Utils::Log::Message("---------------------------------------------------------------");
			OutputRendererDescriptions();
			Utils::Log::Message("---------------------------------------------------------------");
		}
		else if (camera->RendererName() == "BuildIblRenderer")
		{
			camera->SetRendererName("TBForwardRenderer");
			Utils::Log::Message("---------------------------------------------------------------");
			OutputRendererDescriptions();
			Utils::Log::Message("---------------------------------------------------------------");
		}
	}
}

void AirEngine::Test::RendererDataController::OnDestroy()
{
}
