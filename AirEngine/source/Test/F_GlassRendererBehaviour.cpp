#include "Test/F_GlassRendererBehaviour.h"
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
	rttr::registration::class_<AirEngine::Test::F_GlassRendererBehaviour>("AirEngine::Test::F_GlassRendererBehaviour");
}

AirEngine::Test::F_GlassRendererBehaviour::F_GlassRendererBehaviour()
{
}

AirEngine::Test::F_GlassRendererBehaviour::~F_GlassRendererBehaviour()
{
}

void AirEngine::Test::F_GlassRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::F_GlassRendererBehaviour::OnStart()
{
	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Sphere.ply");
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
		auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\F_Opaque_Glass_Shader.shader");
		auto material = new Core::Graphic::Rendering::Material(shader);
		material->SetSampledImageCube("backgroundTexture", background, sampler);
		renderer->AddMaterial(material);
	}

	{
		auto preZShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\Geometry_Shader.shader");
		auto preZMaterial = new Core::Graphic::Rendering::Material(preZShader);
		renderer->AddMaterial(preZMaterial);
	}
}

void AirEngine::Test::F_GlassRendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::F_GlassRendererBehaviour::OnDestroy()
{
}
