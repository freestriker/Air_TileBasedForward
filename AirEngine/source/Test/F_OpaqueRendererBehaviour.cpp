#include "Test/F_OpaqueRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::F_OpaqueRendererBehaviour>("AirEngine::Test::F_OpaqueRendererBehaviour");
}

AirEngine::Test::F_OpaqueRendererBehaviour::F_OpaqueRendererBehaviour()
	: _rotationSpeed(0.5235987755982988f)
{
}

AirEngine::Test::F_OpaqueRendererBehaviour::~F_OpaqueRendererBehaviour()
{
}

void AirEngine::Test::F_OpaqueRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::F_OpaqueRendererBehaviour::OnStart()
{
	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Sphere.ply");
	renderer->mesh = mesh;

	//{
	//	auto diffuseTexture2D = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\WallDiffuseTexture2D.json");
	//	auto normalTexture2D = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\WallNormalTexture2D.json");
	//	auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\OpaqueShader.shader");
	//	auto material = new Core::Graphic::Rendering::Material(shader);
	//	material->SetTexture2D("diffuseTexture", diffuseTexture2D);
	//	material->SetTexture2D("normalTexture", normalTexture2D);
	//	renderer->AddMaterial(material);
	//}

	{
		auto preZShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\GeometryShader.shader");
		auto preZMaterial = new Core::Graphic::Rendering::Material(preZShader);
		renderer->AddMaterial(preZMaterial);
	}

}

void AirEngine::Test::F_OpaqueRendererBehaviour::OnUpdate()
{
	auto rotation = GameObject()->transform.Rotation();
	rotation.x = std::fmod(rotation.x + _rotationSpeed * 0.3f * static_cast<float>(Core::Logic::CoreObject::Instance::time.DeltaDuration()), 6.283185307179586f);
	rotation.y = std::fmod(rotation.y + _rotationSpeed * 0.6f * static_cast<float>(Core::Logic::CoreObject::Instance::time.DeltaDuration()), 6.283185307179586f);
	rotation.z = std::fmod(rotation.z + _rotationSpeed * static_cast<float>(Core::Logic::CoreObject::Instance::time.DeltaDuration()), 6.283185307179586f);
	GameObject()->transform.SetRotation(rotation);
}

void AirEngine::Test::F_OpaqueRendererBehaviour::OnDestroy()
{
}
