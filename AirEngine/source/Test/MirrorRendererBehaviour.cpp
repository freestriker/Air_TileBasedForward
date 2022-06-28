#include "Test/MirrorRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::MirrorRendererBehaviour>("AirEngine::Test::MirrorRendererBehaviour");
}

AirEngine::Test::MirrorRendererBehaviour::MirrorRendererBehaviour()
	: meshTask()
	, shaderTask()
	, loaded(false)
	, mesh(nullptr)
	, shader(nullptr)
	, material(nullptr)
{
}

AirEngine::Test::MirrorRendererBehaviour::~MirrorRendererBehaviour()
{
}

void AirEngine::Test::MirrorRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::MirrorRendererBehaviour::OnStart()
{
	meshTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	shaderTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Core::Graphic::Shader>("..\\Asset\\Shader\\MirrorShader.shader");
}

void AirEngine::Test::MirrorRendererBehaviour::OnUpdate()
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

void AirEngine::Test::MirrorRendererBehaviour::OnDestroy()
{
}
