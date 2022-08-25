#include "Test/F_GlassRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::F_GlassRendererBehaviour>("AirEngine::Test::F_GlassRendererBehaviour");
}

AirEngine::Test::F_GlassRendererBehaviour::F_GlassRendererBehaviour()
{
}

AirEngine::Test::F_GlassRendererBehaviour::~F_GlassRendererBehaviour()
{
}

void AirEngine::Test::F_GlassRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::F_GlassRendererBehaviour::OnStart()
{
	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Sphere.ply");
	renderer->mesh = mesh;

	{
		auto background = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
		auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Shader>("..\\Asset\\Shader\\GlassShader.shader");
		auto material = new Core::Graphic::Material(shader);
		material->SetTextureCube("backgroundTexture", background);
		renderer->AddMaterial(material);
	}

	{
		auto preZShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Shader>("..\\Asset\\Shader\\GeometryShader.shader");
		auto preZMaterial = new Core::Graphic::Material(preZShader);
		renderer->AddMaterial(preZMaterial);
	}
}

void AirEngine::Test::F_GlassRendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::F_GlassRendererBehaviour::OnDestroy()
{
}
