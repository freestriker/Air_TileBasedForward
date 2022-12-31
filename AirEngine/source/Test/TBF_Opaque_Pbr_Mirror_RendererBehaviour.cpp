#include "Test/TBF_Opaque_Pbr_Mirror_RendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::TBF_Opaque_Pbr_Mirror_RendererBehaviour>("AirEngine::Test::TBF_Opaque_Pbr_Mirror_RendererBehaviour");
}

AirEngine::Test::TBF_Opaque_Pbr_Mirror_RendererBehaviour::TBF_Opaque_Pbr_Mirror_RendererBehaviour()
{
}

AirEngine::Test::TBF_Opaque_Pbr_Mirror_RendererBehaviour::~TBF_Opaque_Pbr_Mirror_RendererBehaviour()
{
}

void AirEngine::Test::TBF_Opaque_Pbr_Mirror_RendererBehaviour::OnAwake()
{
}

void AirEngine::Test::TBF_Opaque_Pbr_Mirror_RendererBehaviour::OnStart()
{
	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Complex.ply");
	renderer->mesh = mesh;
	auto sampler = new Core::Graphic::Instance::ImageSampler(
		VkFilter::VK_FILTER_NEAREST,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	);

	{
		auto background = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>("..\\Asset\\Texture\\DefaultTextureCube.json");
		auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\TBF_Opaque_Pbr_Mirror_Shader.shader");
		auto material = new Core::Graphic::Rendering::Material(shader);
		renderer->AddMaterial(material);
	}

	{
		auto geometryShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\Geometry_Shader.shader");
		auto geometryMaterial = new Core::Graphic::Rendering::Material(geometryShader);
		renderer->AddMaterial(geometryMaterial);
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

void AirEngine::Test::TBF_Opaque_Pbr_Mirror_RendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::TBF_Opaque_Pbr_Mirror_RendererBehaviour::OnDestroy()
{
}
