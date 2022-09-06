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
}

void AirEngine::Test::RendererDataController::OnDestroy()
{
}
