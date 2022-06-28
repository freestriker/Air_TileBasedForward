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
	, rotationSpeed(0.5235987755982988f)
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
	meshTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\SquareMesh.ply");
	shaderTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Core::Graphic::Shader>("..\\Asset\\Shader\\DefaultTransparentShader.shader");
	diffuseTexture2DTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Texture2D>("..\\Asset\\Texture\\BrokenGlassTexture2d.json");
}

void AirEngine::Test::TransparentRendererBehaviour::OnUpdate()
{
	if (!loaded && meshTask._Is_ready() && shaderTask._Is_ready() && diffuseTexture2DTask._Is_ready())
	{
		mesh = meshTask.get();
		shader = shaderTask.get();
		diffuseTexture2D = diffuseTexture2DTask.get();
		material = new Core::Graphic::Material(shader);
		material->SetTexture2D("albedo", diffuseTexture2D);

		loaded = true;

		auto meshRenderer = GameObject()->GetComponent<Renderer::Renderer>();
		meshRenderer->material = material;
		meshRenderer->mesh = mesh;
		Utils::Log::Message("Finish load.");
	}

	auto rotation = GameObject()->transform.Rotation();
	rotation.x = std::fmod(rotation.x + rotationSpeed * 0.3f * static_cast<float>(Core::Logic::CoreObject::Instance::time.DeltaDuration()), 6.283185307179586f);
	rotation.y = std::fmod(rotation.y + rotationSpeed * 0.6f * static_cast<float>(Core::Logic::CoreObject::Instance::time.DeltaDuration()), 6.283185307179586f);
	rotation.z = std::fmod(rotation.z + rotationSpeed * static_cast<float>(Core::Logic::CoreObject::Instance::time.DeltaDuration()), 6.283185307179586f);
	GameObject()->transform.SetRotation(rotation);
}

void AirEngine::Test::TransparentRendererBehaviour::OnDestroy()
{
}
