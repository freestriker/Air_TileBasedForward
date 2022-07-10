#include "Test/TransparentRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::TransparentRendererBehaviour>("AirEngine::Test::TransparentRendererBehaviour");
}

AirEngine::Test::TransparentRendererBehaviour::TransparentRendererBehaviour()
	: meshTask()
	, shaderTask()
	, diffuseTexture2DTask()
	, loaded(false)
	, mesh(nullptr)
	, shader(nullptr)
	, diffuseTexture2D(nullptr)
	, material(nullptr)
{
}

AirEngine::Test::TransparentRendererBehaviour::~TransparentRendererBehaviour()
{
}

void AirEngine::Test::TransparentRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::TransparentRendererBehaviour::OnStart()
{
	meshTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\Square.ply");
	shaderTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Core::Graphic::Shader>("..\\Asset\\Shader\\TransparentShader.shader");
	diffuseTexture2DTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Texture2D>("..\\Asset\\Texture\\BrokenGlassTexture2D.json");
}

void AirEngine::Test::TransparentRendererBehaviour::OnUpdate()
{
	if (!loaded && meshTask._Is_ready() && shaderTask._Is_ready() && diffuseTexture2DTask._Is_ready())
	{
		mesh = meshTask.get();
		shader = shaderTask.get();
		diffuseTexture2D = diffuseTexture2DTask.get();
		material = new Core::Graphic::Material(shader);
		material->SetTexture2D("diffuseTexture", diffuseTexture2D);

		loaded = true;

		auto meshRenderer = GameObject()->GetComponent<Renderer::Renderer>();
		meshRenderer->AddMaterial(material);
		meshRenderer->mesh = mesh;
		Utils::Log::Message("Finish load.");
	}
}

void AirEngine::Test::TransparentRendererBehaviour::OnDestroy()
{
}
