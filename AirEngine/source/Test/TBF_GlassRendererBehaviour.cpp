#include "Test/TBF_GlassRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::TBF_GlassRendererBehaviour>("AirEngine::Test::TBF_GlassRendererBehaviour");
}

AirEngine::Test::TBF_GlassRendererBehaviour::TBF_GlassRendererBehaviour()
{
}

AirEngine::Test::TBF_GlassRendererBehaviour::~TBF_GlassRendererBehaviour()
{
}

void AirEngine::Test::TBF_GlassRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::TBF_GlassRendererBehaviour::OnStart()
{
	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Sphere.ply");
	renderer->mesh = mesh;

	{
		auto background = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
		auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\TBF_Opaque_Glass_Shader.shader");
		auto material = new Core::Graphic::Rendering::Material(shader);
		material->SetTextureCube("backgroundTexture", background);
		renderer->AddMaterial(material);
	}

	{
		auto preZShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\GeometryShader.shader");
		auto preZMaterial = new Core::Graphic::Rendering::Material(preZShader);
		renderer->AddMaterial(preZMaterial);
	}
}

void AirEngine::Test::TBF_GlassRendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::TBF_GlassRendererBehaviour::OnDestroy()
{
}
