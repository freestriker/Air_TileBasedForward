#include "Test/QuadMoveBehaviour.h"
#include "Audio/AudioSource.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Asset/AudioClip.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include <algorithm>
#include "Core/Logic/Manager/InputManager.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Asset/Mesh.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/Rendering/Material.h"
#include <future>
#include "Renderer/Renderer.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::QuadMoveBehaviour>("AirEngine::Test::QuadMoveBehaviour");
}

AirEngine::Test::QuadMoveBehaviour::QuadMoveBehaviour()
{
}

AirEngine::Test::QuadMoveBehaviour::~QuadMoveBehaviour()
{
}

void AirEngine::Test::QuadMoveBehaviour::OnAwake()
{
}

void AirEngine::Test::QuadMoveBehaviour::OnStart()
{
	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\LargeQuad.ply");
	renderer->mesh = mesh;
	auto sampler = new Core::Graphic::Instance::ImageSampler(
		VkFilter::VK_FILTER_NEAREST,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	);

	{
		auto diffuseTexture2D = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>("..\\Asset\\Texture\\WallDiffuseTexture2D.json");
		auto normalTexture2D = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>("..\\Asset\\Texture\\WallNormalTexture2D.json");
		auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\TBF_Opaque_Wall_Shader.shader");
		auto material = new Core::Graphic::Rendering::Material(shader);
		material->SetSampledImage2D("diffuseTexture", diffuseTexture2D, sampler);
		material->SetSampledImage2D("normalTexture", normalTexture2D, sampler);
		renderer->AddMaterial(material);
	}

	{
		auto preZShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\Geometry_Shader.shader");
		auto preZMaterial = new Core::Graphic::Rendering::Material(preZShader);
		renderer->AddMaterial(preZMaterial);
	}

	{
		auto shadowCasterShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\CSM_ShadowCaster_Shader.shader");
		auto shadowCasterMaterial = new Core::Graphic::Rendering::Material(shadowCasterShader);
		renderer->AddMaterial(shadowCasterMaterial);
	}

	{
		auto shadowCasterShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\CascadeEVSM_ShadowCaster_Shader.shader");
		auto shadowCasterMaterial = new Core::Graphic::Rendering::Material(shadowCasterShader);
		renderer->AddMaterial(shadowCasterMaterial);
	}
}

void AirEngine::Test::QuadMoveBehaviour::OnUpdate()
{
	float moveSpeed = 1;
	if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_Q) == AirEngine::Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		auto t = GameObject()->transform.Translation();
		t.z += moveSpeed * Core::Logic::CoreObject::Instance::time.DeltaDuration();
		GameObject()->transform.SetTranslation(t);
	}
	else if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_E) == AirEngine::Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		auto t = GameObject()->transform.Translation();
		t.z -= moveSpeed * Core::Logic::CoreObject::Instance::time.DeltaDuration();
		GameObject()->transform.SetTranslation(t);
	}
}

void AirEngine::Test::QuadMoveBehaviour::OnDestroy()
{
}
