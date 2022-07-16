#include "Test/TBF_OIT_RedBoxBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Renderer/Renderer.h"

AirEngine::Test::TBF_OIT_RedBoxBehaviour::TBF_OIT_RedBoxBehaviour()
	: _material(nullptr)
{
}

AirEngine::Test::TBF_OIT_RedBoxBehaviour::~TBF_OIT_RedBoxBehaviour()
{
}

void AirEngine::Test::TBF_OIT_RedBoxBehaviour::OnAwake()
{
}

void AirEngine::Test::TBF_OIT_RedBoxBehaviour::OnStart()
{
	auto boxMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Box.ply");
	auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Shader>("..\\Asset\\Shader\\TBF_OIT_DP_RedShader.shader");

	_material = new AirEngine::Core::Graphic::Material(shader);

	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	renderer->AddMaterial(_material);
	renderer->mesh = boxMesh;
}

void AirEngine::Test::TBF_OIT_RedBoxBehaviour::OnUpdate()
{
}

void AirEngine::Test::TBF_OIT_RedBoxBehaviour::OnDestroy()
{
}
