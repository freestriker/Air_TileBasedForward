#include "Test/F_WallRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::F_WallRendererBehaviour>("AirEngine::Test::F_WallRendererBehaviour");
}

AirEngine::Test::F_WallRendererBehaviour::F_WallRendererBehaviour()
{
}

AirEngine::Test::F_WallRendererBehaviour::~F_WallRendererBehaviour()
{
}

void AirEngine::Test::F_WallRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::F_WallRendererBehaviour::OnStart()
{
	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Sphere.ply");
	renderer->mesh = mesh;

	{
		auto diffuseTexture2D = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\WallDiffuseTexture2D.json");
		auto normalTexture2D = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\WallNormalTexture2D.json");
		auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\F_Opaque_Wall_Shader.shader");
		auto material = new Core::Graphic::Rendering::Material(shader);
		material->SetTexture2D("diffuseTexture", diffuseTexture2D);
		material->SetTexture2D("normalTexture", normalTexture2D);
		renderer->AddMaterial(material);
	}

	{
		auto preZShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\Geometry_Shader.shader");
		auto preZMaterial = new Core::Graphic::Rendering::Material(preZShader);
		renderer->AddMaterial(preZMaterial);
	}
}

void AirEngine::Test::F_WallRendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::F_WallRendererBehaviour::OnDestroy()
{
}
