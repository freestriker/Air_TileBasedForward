#include "Test/BackgroundRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Renderer/Renderer.h"

void AirEngine::Test::BackgroundRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::BackgroundRendererBehaviour::OnStart()
{
	meshTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\SkyBoxMesh.ply");
	shaderTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Core::Graphic::Shader>("..\\Asset\\Shader\\DefaultBackgroundShader.shader");
	textureCubeTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
}

void AirEngine::Test::BackgroundRendererBehaviour::OnUpdate()
{
	if (!loaded && meshTask._Is_ready() && shaderTask._Is_ready() && textureCubeTask._Is_ready())
	{
		auto meshRenderer = GameObject()->GetComponent<Renderer::Renderer>();

		mesh = meshTask.get();
		shader = shaderTask.get();
		textureCube = textureCubeTask.get();
		material = new Core::Graphic::Material(shader);
		material->SetTextureCube("backgroundTexture", textureCube);

		loaded = true;

		meshRenderer->material = material;
		meshRenderer->mesh = mesh;
		Utils::Log::Message("Finish load.");
	}
}

void AirEngine::Test::BackgroundRendererBehaviour::OnDestroy()
{
}

AirEngine::Test::BackgroundRendererBehaviour::BackgroundRendererBehaviour()
	: meshTask()
	, shaderTask()
	, textureCubeTask()
	, loaded(false)
	, mesh(nullptr)
	, shader(nullptr)
	, textureCube(nullptr)
	, material(nullptr)
{
}

AirEngine::Test::BackgroundRendererBehaviour::~BackgroundRendererBehaviour()
{
}
