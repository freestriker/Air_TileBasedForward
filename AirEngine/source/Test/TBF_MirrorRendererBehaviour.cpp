#include "Test/TBF_MirrorRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::TBF_MirrorRendererBehaviour>("AirEngine::Test::TBF_MirrorRendererBehaviour");
}

AirEngine::Test::TBF_MirrorRendererBehaviour::TBF_MirrorRendererBehaviour()
{
}

AirEngine::Test::TBF_MirrorRendererBehaviour::~TBF_MirrorRendererBehaviour()
{
}

void AirEngine::Test::TBF_MirrorRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::TBF_MirrorRendererBehaviour::OnStart()
{
	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Complex.ply");
	renderer->mesh = mesh;

	{
		auto background = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
		auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\TBF_Opaque_Mirror_Shader.shader");
		auto material = new Core::Graphic::Rendering::Material(shader);
		material->SetTextureCube("backgroundTexture", background);
		renderer->AddMaterial(material);
	}

	{
		auto preZShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\Geometry_Shader.shader");
		auto preZMaterial = new Core::Graphic::Rendering::Material(preZShader);
		renderer->AddMaterial(preZMaterial);
	}
}

void AirEngine::Test::TBF_MirrorRendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::TBF_MirrorRendererBehaviour::OnDestroy()
{
}
