#include "Test/Background_SkyboxRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Renderer/Renderer.h"
#include "Core/Graphic/Rendering/Material.h"
#include "Core/Graphic/Rendering/Shader.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::Background_SkyboxRendererBehaviour>("AirEngine::Test::Background_SkyboxRendererBehaviour");
}


void AirEngine::Test::Background_SkyboxRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::Background_SkyboxRendererBehaviour::OnStart()
{
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply");
	auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\Background_Skybox_Shader.shader");
	auto background = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");

	auto material = new Core::Graphic::Rendering::Material(shader);
	material->SetTextureCube("backgroundTexture", background);

	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	renderer->AddMaterial(material);
	renderer->mesh = mesh;
}

void AirEngine::Test::Background_SkyboxRendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::Background_SkyboxRendererBehaviour::OnDestroy()
{
}

AirEngine::Test::Background_SkyboxRendererBehaviour::Background_SkyboxRendererBehaviour()
{
}

AirEngine::Test::Background_SkyboxRendererBehaviour::~Background_SkyboxRendererBehaviour()
{
}
