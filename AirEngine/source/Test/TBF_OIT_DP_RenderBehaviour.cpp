#include "Test/TBF_OIT_DP_RenderBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Renderer/Renderer.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::TBF_OIT_DP_RenderBehaviour>("AirEngine::Test::TBF_OIT_DP_RenderBehaviour");
}


AirEngine::Test::TBF_OIT_DP_RenderBehaviour::TBF_OIT_DP_RenderBehaviour(std::string diffuseTextureName)
	: _diffuseTextureName(diffuseTextureName)
{
}

AirEngine::Test::TBF_OIT_DP_RenderBehaviour::TBF_OIT_DP_RenderBehaviour()
	: _diffuseTextureName()
{
}

AirEngine::Test::TBF_OIT_DP_RenderBehaviour::~TBF_OIT_DP_RenderBehaviour()
{
}

void AirEngine::Test::TBF_OIT_DP_RenderBehaviour::OnAwake()
{
}

void AirEngine::Test::TBF_OIT_DP_RenderBehaviour::OnStart()
{
	auto boxMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Complex.ply");
	auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\TBF_OIT_DepthPeeling_Shader.shader");
	auto diffuse = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\" + _diffuseTextureName + ".json");

	auto material = new AirEngine::Core::Graphic::Rendering::Material(shader);
	material->SetTexture2D("diffuseTexture", diffuse);

	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	renderer->AddMaterial(material);
	renderer->mesh = boxMesh;
}

void AirEngine::Test::TBF_OIT_DP_RenderBehaviour::OnUpdate()
{
}

void AirEngine::Test::TBF_OIT_DP_RenderBehaviour::OnDestroy()
{
}
