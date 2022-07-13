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
	, backgroundTextureTask()
	, backgroundTexture(nullptr)
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
	meshTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\Sphere.ply");
	backgroundTextureTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
	shaderTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Core::Graphic::Shader>("..\\Asset\\Shader\\MirrorShader.shader");
	{
		auto preZShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Shader>("..\\Asset\\Shader\\PreZShader.shader");
		auto preZMaterial = new Core::Graphic::Material(preZShader);
		GameObject()->GetComponent<Renderer::Renderer>()->AddMaterial(preZMaterial);
	}
}

void AirEngine::Test::MirrorRendererBehaviour::OnUpdate()
{
	if (!loaded && meshTask._Is_ready() && backgroundTextureTask._Is_ready() && shaderTask._Is_ready())
	{
		auto meshRenderer = GameObject()->GetComponent<Renderer::Renderer>();

		mesh = meshTask.get();
		shader = shaderTask.get();
		backgroundTexture = backgroundTextureTask.get();
		material = new Core::Graphic::Material(shader);

		loaded = true;

		meshRenderer->AddMaterial(material);
		meshRenderer->mesh = mesh;
		material->SetTextureCube("backgroundTexture", backgroundTexture);
		Utils::Log::Message("Finish load.");
	}
}

void AirEngine::Test::MirrorRendererBehaviour::OnDestroy()
{
}
