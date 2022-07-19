#include "Test/TBF_TransparentRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::TBF_TransparentRendererBehaviour>("AirEngine::Test::TBF_TransparentRendererBehaviour");
}

AirEngine::Test::TBF_TransparentRendererBehaviour::TBF_TransparentRendererBehaviour()
{
}

AirEngine::Test::TBF_TransparentRendererBehaviour::~TBF_TransparentRendererBehaviour()
{
}

void AirEngine::Test::TBF_TransparentRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::TBF_TransparentRendererBehaviour::OnStart()
{
	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Square.ply");
	renderer->mesh = mesh;

	{
		auto diffuse = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\BrokenGlassTexture2D.json");
		auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Shader>("..\\Asset\\Shader\\TBF_TransparentShader.shader");
		auto material = new Core::Graphic::Material(shader);
		material->SetTexture2D("diffuseTexture", diffuse);
		renderer->AddMaterial(material);
	}
}

void AirEngine::Test::TBF_TransparentRendererBehaviour::OnUpdate()
{
}

void AirEngine::Test::TBF_TransparentRendererBehaviour::OnDestroy()
{
}
