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
#include "Asset/Texture2D.h"
#include "Asset/TextureCube.h"
#include "Core/Graphic/Rendering/Material.h"
#include <future>
#include "Renderer/Renderer.h"
#include "Camera/CameraBase.h"
#include "Rendering/Renderer/TBForwardRenderer.h"
#include "Rendering/Renderer/AmbientOcclusionRenderer.h"
#include "Rendering/Renderer/ShadowVisualizationRenderer.h"

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

void AirEngine::Test::RendererDataController::OnStart()
{

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

	///Shadow type
	{
		auto rendererData = dynamic_cast<Rendering::Renderer::ShadowVisualizationRenderer::ShadowVisualizationRendererData*>(rendererDataBase);
		if (rendererData != nullptr)
		{
			if (Core::Logic::CoreObject::Instance::InputManager().KeyUp(Core::Logic::Manager::InputKeyType::Key_J))
			{
				if (rendererData->shadowType == Rendering::Renderer::ShadowVisualizationRenderer::ShadowType::CSM)
				{
					rendererData->shadowType = Rendering::Renderer::ShadowVisualizationRenderer::ShadowType::CASCADE_EVSM;
					Utils::Log::Message("Switch Shadow mode to CASCADE_EVSM.");
				}
				else if (rendererData->shadowType == Rendering::Renderer::ShadowVisualizationRenderer::ShadowType::CASCADE_EVSM)
				{
					rendererData->shadowType = Rendering::Renderer::ShadowVisualizationRenderer::ShadowType::CSM;
					Utils::Log::Message("Switch Shadow mode to CSM.");
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
			Utils::Log::Message("Switch Renderer mode to AmbientOcclusionRenderer.");
		}
		else if (camera->RendererName() == "AmbientOcclusionRenderer")
		{
			camera->SetRendererName("ShadowVisualizationRenderer");
			Utils::Log::Message("Switch Renderer mode to ShadowVisualizationRenderer.");
		}
		else if (camera->RendererName() == "ShadowVisualizationRenderer")
		{
			camera->SetRendererName("TBForwardRenderer");
			Utils::Log::Message("Switch Renderer mode to TBForwardRenderer.");
		}
	}
}

void AirEngine::Test::RendererDataController::OnDestroy()
{
}
