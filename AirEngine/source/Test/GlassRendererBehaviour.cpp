#include "Test/GlassRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::GlassRendererBehaviour>("AirEngine::Test::GlassRendererBehaviour");
}

AirEngine::Test::GlassRendererBehaviour::GlassRendererBehaviour()
	: meshTask()
	, shaderTask()
	, loaded(false)
	, mesh(nullptr)
	, shader(nullptr)
	, material(nullptr)
{
}

AirEngine::Test::GlassRendererBehaviour::~GlassRendererBehaviour()
{
}

void AirEngine::Test::GlassRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::GlassRendererBehaviour::OnStart()
{
	meshTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	shaderTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Core::Graphic::Shader>("..\\Asset\\Shader\\GlassShader.shader");
}

void AirEngine::Test::GlassRendererBehaviour::OnUpdate()
{
	if (!loaded && meshTask._Is_ready() && shaderTask._Is_ready())
	{
		auto meshRenderer = GameObject()->GetComponent<Renderer::Renderer>();

		mesh = meshTask.get();
		shader = shaderTask.get();
		material = new Core::Graphic::Material(shader);

		loaded = true;

		meshRenderer->material = material;
		meshRenderer->mesh = mesh;
		Utils::Log::Message("Finish load.");
	}
}

void AirEngine::Test::GlassRendererBehaviour::OnDestroy()
{
}
