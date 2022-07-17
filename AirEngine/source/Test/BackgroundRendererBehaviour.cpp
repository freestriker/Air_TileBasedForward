#include "Test/BackgroundRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Renderer/Renderer.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::BackgroundRendererBehaviour>("AirEngine::Test::BackgroundRendererBehaviour");
}


void AirEngine::Test::BackgroundRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::BackgroundRendererBehaviour::OnStart()
{
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply");
	auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Shader>("..\\Asset\\Shader\\BackgroundShader.shader");
	auto background = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");

	auto material = new Core::Graphic::Material(shader);
	material->SetTextureCube("backgroundTexture", background);

	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	renderer->AddMaterial(material);
	renderer->mesh = mesh;
}

void AirEngine::Test::BackgroundRendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::BackgroundRendererBehaviour::OnDestroy()
{
}

AirEngine::Test::BackgroundRendererBehaviour::BackgroundRendererBehaviour()
{
}

AirEngine::Test::BackgroundRendererBehaviour::~BackgroundRendererBehaviour()
{
}
