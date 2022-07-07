#include "Test/OpaqueRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::OpaqueRendererBehaviour>("AirEngine::Test::OpaqueRendererBehaviour");
}

AirEngine::Test::OpaqueRendererBehaviour::OpaqueRendererBehaviour()
	: meshTask()
	, shaderTask()
	, diffuseTexture2DTask()
	, normalTexture2DTask()
	, loaded(false)
	, mesh(nullptr)
	, shader(nullptr)
	, diffuseTexture2D(nullptr)
	, normalTexture2D(nullptr)
	, material(nullptr)
	, rotationSpeed(0.5235987755982988f)
{
}

AirEngine::Test::OpaqueRendererBehaviour::~OpaqueRendererBehaviour()
{
}

void AirEngine::Test::OpaqueRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::OpaqueRendererBehaviour::OnStart()
{
	meshTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\Sphere.ply");
	shaderTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Core::Graphic::Shader>("..\\Asset\\Shader\\OpaqueShader.shader");
	diffuseTexture2DTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Texture2D>("..\\Asset\\Texture\\WallDiffuseTexture2D.json");
	normalTexture2DTask = Core::IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Texture2D>("..\\Asset\\Texture\\WallNormalTexture2D.json");
}

void AirEngine::Test::OpaqueRendererBehaviour::OnUpdate()
{
	if (!loaded && meshTask._Is_ready() && shaderTask._Is_ready() && diffuseTexture2DTask._Is_ready() && normalTexture2DTask._Is_ready())
	{
		auto meshRenderer = GameObject()->GetComponent<Renderer::Renderer>();
		
		mesh = meshTask.get();
		shader = shaderTask.get();
		diffuseTexture2D = diffuseTexture2DTask.get();
		normalTexture2D = normalTexture2DTask.get();
		material = new Core::Graphic::Material(shader);
		material->SetTexture2D("diffuseTexture", diffuseTexture2D);
		material->SetTexture2D("normalTexture", normalTexture2D);

		loaded = true;

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

void AirEngine::Test::OpaqueRendererBehaviour::OnDestroy()
{
}
