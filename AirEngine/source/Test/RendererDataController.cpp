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
	if (Core::Logic::CoreObject::Instance::InputManager().KeyDown(Core::Logic::Manager::InputKeyType::Key_A))
	{
		Utils::Log::Message("Key: A is down.");
	}
	if (Core::Logic::CoreObject::Instance::InputManager().MouseDown(Core::Logic::Manager::InputMouseType::LeftButton))
	{
		Utils::Log::Message("Mouse: Left button is down.");
	}

	auto camera = GameObject()->GetComponent<Camera::CameraBase>();
	auto rendererData = dynamic_cast<Rendering::Renderer::TBForwardRenderer::TBForwardRendererData*>(camera->RendererData());
	if (rendererData)
	{
		if (Core::Logic::CoreObject::Instance::InputManager().KeyUp(Core::Logic::Manager::InputKeyType::Key_J))
		{
			if (rendererData->oitType == Rendering::Renderer::TBForwardRenderer::OitType::DEPTH_PEELING)
			{
				rendererData->oitType = Rendering::Renderer::TBForwardRenderer::OitType::ALPHA_BUFFER;
				Utils::Log::Message("Switch OIT mode to Alpha-Buffer.");
			}
			else
			{
				rendererData->oitType = Rendering::Renderer::TBForwardRenderer::OitType::DEPTH_PEELING;
				Utils::Log::Message("Switch OIT mode to Depth-Peeling.");
			}
		}
		if (Core::Logic::CoreObject::Instance::InputManager().KeyUp(Core::Logic::Manager::InputKeyType::Key_K))
		{
			if (rendererData->aoType == Rendering::Renderer::TBForwardRenderer::AoType::SSAO)
			{
				rendererData->aoType = Rendering::Renderer::TBForwardRenderer::AoType::HBAO;
				Utils::Log::Message("Switch AO mode to HBAO.");
			}
			else if (rendererData->aoType == Rendering::Renderer::TBForwardRenderer::AoType::HBAO)
			{
				rendererData->aoType = Rendering::Renderer::TBForwardRenderer::AoType::GTAO;
				Utils::Log::Message("Switch AO mode to GTAO.");
			}
			else
			{
				rendererData->aoType = Rendering::Renderer::TBForwardRenderer::AoType::SSAO;
				Utils::Log::Message("Switch AO mode to SSAO.");
			}
		}
	}
	else
	{
		auto rendererData = dynamic_cast<Rendering::Renderer::AmbientOcclusionRenderer::AmbientOcclusionRendererData*>(camera->RendererData());
		if (Core::Logic::CoreObject::Instance::InputManager().KeyUp(Core::Logic::Manager::InputKeyType::Key_K))
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
			else
			{
				rendererData->aoType = Rendering::Renderer::AmbientOcclusionRenderer::AoType::SSAO;
				Utils::Log::Message("Switch AO mode to SSAO.");
			}
		}
	}
	if (Core::Logic::CoreObject::Instance::InputManager().KeyUp(Core::Logic::Manager::InputKeyType::Key_L))
	{
		if (camera->RendererName() == "TBForwardRenderer")
		{
			camera->SetRendererName("AmbientOcclusionRenderer");
			Utils::Log::Message("Switch Renderer mode to AmbientOcclusionRenderer.");
		}
		else
		{
			camera->SetRendererName("TBForwardRenderer");
			Utils::Log::Message("Switch Renderer mode to TBForwardRenderer.");
		}
	}
}

void AirEngine::Test::RendererDataController::OnDestroy()
{
}
