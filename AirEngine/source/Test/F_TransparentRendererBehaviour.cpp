#include "Test/F_TransparentRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::F_TransparentRendererBehaviour>("AirEngine::Test::F_TransparentRendererBehaviour");
}

AirEngine::Test::F_TransparentRendererBehaviour::F_TransparentRendererBehaviour()
{
}

AirEngine::Test::F_TransparentRendererBehaviour::~F_TransparentRendererBehaviour()
{
}

void AirEngine::Test::F_TransparentRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::F_TransparentRendererBehaviour::OnStart()
{
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Square.ply");
	auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Shader>("..\\Asset\\Shader\\F_TransparentShader.shader");
	auto diffuse = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\BrokenGlassTexture2D.json");

	auto material = new Core::Graphic::Material(shader);
	material->SetTexture2D("diffuseTexture", diffuse);

	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	renderer->AddMaterial(material);
	renderer->mesh = mesh;
}

void AirEngine::Test::F_TransparentRendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::F_TransparentRendererBehaviour::OnDestroy()
{
}
