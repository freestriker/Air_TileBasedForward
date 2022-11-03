#include "Test/Background_SkyboxRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Renderer/Renderer.h"
#include "Core/Graphic/Rendering/Material.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::Background_SkyboxRendererBehaviour>("AirEngine::Test::Background_SkyboxRendererBehaviour");
}


void AirEngine::Test::Background_SkyboxRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::Background_SkyboxRendererBehaviour::OnStart()
{
	//auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply");
	//auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\Background_Hdr_Shader.shader");
	//auto background = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>("..\\Asset\\Texture\\WorkShop.json");
	//auto sampler = new Core::Graphic::Instance::ImageSampler(
	//	VkFilter::VK_FILTER_LINEAR,
	//	VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
	//	VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
	//	0.0f,
	//	VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	//);
	//auto material = new Core::Graphic::Rendering::Material(shader);
	//material->SetSampledImage2D("backgroundTexture", background, sampler);
	//auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	//renderer->AddMaterial(material);
	//renderer->mesh = mesh;

	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\BackgroundMesh.ply");
	auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\Background_Skybox_Shader.shader");
	auto background = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>("..\\Asset\\Texture\\SkyImageCube.json");
	auto sampler = new Core::Graphic::Instance::ImageSampler(
		VkFilter::VK_FILTER_NEAREST,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	);
	auto material = new Core::Graphic::Rendering::Material(shader);
	material->SetSampledImageCube("backgroundTexture", background, sampler);
	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	renderer->AddMaterial(material);
	renderer->mesh = mesh;
}

void AirEngine::Test::Background_SkyboxRendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::Background_SkyboxRendererBehaviour::OnDestroy()
{
}

AirEngine::Test::Background_SkyboxRendererBehaviour::Background_SkyboxRendererBehaviour()
{
}

AirEngine::Test::Background_SkyboxRendererBehaviour::~Background_SkyboxRendererBehaviour()
{
}
