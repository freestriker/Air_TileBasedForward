#include "Test/F_BrokenGlassRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"
#include "Core/Graphic/Rendering/Material.h"
#include "Core/Graphic/Rendering/Shader.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::F_BrokenGlassRendererBehaviour>("AirEngine::Test::F_BrokenGlassRendererBehaviour");
}

AirEngine::Test::F_BrokenGlassRendererBehaviour::F_BrokenGlassRendererBehaviour()
{
}

AirEngine::Test::F_BrokenGlassRendererBehaviour::~F_BrokenGlassRendererBehaviour()
{
}

void AirEngine::Test::F_BrokenGlassRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::F_BrokenGlassRendererBehaviour::OnStart()
{
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Square.ply");
	auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\F_Transparent_BrokenGlass_Shader.shader");
	auto diffuse = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\BrokenGlassTexture2D.json");

	auto material = new Core::Graphic::Rendering::Material(shader);
	material->SetTexture2D("diffuseTexture", diffuse);

	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	renderer->AddMaterial(material);
	renderer->mesh = mesh;
}

void AirEngine::Test::F_BrokenGlassRendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::F_BrokenGlassRendererBehaviour::OnDestroy()
{
}
