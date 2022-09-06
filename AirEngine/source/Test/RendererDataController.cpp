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
	auto rendererData = static_cast<Rendering::Renderer::TBForwardRenderer::TBForwardRendererData*>(camera->RendererData());
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

void AirEngine::Test::RendererDataController::OnDestroy()
{
}
