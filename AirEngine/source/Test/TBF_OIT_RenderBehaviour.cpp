#include "Test/TBF_OIT_RenderBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Renderer/Renderer.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::TBF_OIT_RenderBehaviour>("AirEngine::Test::TBF_OIT_DP_RenderBehaviour");
}


AirEngine::Test::TBF_OIT_RenderBehaviour::TBF_OIT_RenderBehaviour(std::string diffuseTextureName)
	: _diffuseTextureName(diffuseTextureName)
{
}

AirEngine::Test::TBF_OIT_RenderBehaviour::TBF_OIT_RenderBehaviour()
	: _diffuseTextureName()
{
}

AirEngine::Test::TBF_OIT_RenderBehaviour::~TBF_OIT_RenderBehaviour()
{
}

void AirEngine::Test::TBF_OIT_RenderBehaviour::OnAwake()
{
}

void AirEngine::Test::TBF_OIT_RenderBehaviour::OnStart()
{
	auto boxMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Complex.ply");
	auto dpShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\TBF_OIT_DepthPeeling_Shader.shader");
	auto abShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\TBF_OIT_AlphaBuffer_Shader.shader");
	auto tShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\TBF_Transparent_Shader.shader");

	auto dpMaterial = new AirEngine::Core::Graphic::Rendering::Material(dpShader);

	auto abMaterial = new AirEngine::Core::Graphic::Rendering::Material(abShader);

	auto tMaterial = new AirEngine::Core::Graphic::Rendering::Material(tShader);

	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	renderer->AddMaterial(dpMaterial);
	renderer->AddMaterial(abMaterial);
	renderer->AddMaterial(tMaterial);
	renderer->mesh = boxMesh;
}

void AirEngine::Test::TBF_OIT_RenderBehaviour::OnUpdate()
{
}

void AirEngine::Test::TBF_OIT_RenderBehaviour::OnDestroy()
{
}
