#include "Test/F_BrokenGlassRendererBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"
#include "Core/Graphic/Rendering/Material.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::F_BrokenGlassRendererBehaviour>("AirEngine::Test::F_BrokenGlassRendererBehaviour");
}

AirEngine::Test::F_BrokenGlassRendererBehaviour::F_BrokenGlassRendererBehaviour()
{
}

AirEngine::Test::F_BrokenGlassRendererBehaviour::~F_BrokenGlassRendererBehaviour()
{
}

void AirEngine::Test::F_BrokenGlassRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::F_BrokenGlassRendererBehaviour::OnStart()
{
	auto mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Square.ply");
	auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\F_Transparent_BrokenGlass_Shader.shader");
	auto diffuse = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>("..\\Asset\\Texture\\BrokenGlassTexture2D.json");
	auto sampler = new Core::Graphic::Instance::ImageSampler(
		VkFilter::VK_FILTER_NEAREST,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_NEAREST,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	);

	auto material = new Core::Graphic::Rendering::Material(shader);
	material->SetSampledImage2D("diffuseTexture", diffuse, sampler);

	auto renderer = GameObject()->GetComponent<Renderer::Renderer>();
	renderer->AddMaterial(material);
	renderer->mesh = mesh;
}

void AirEngine::Test::F_BrokenGlassRendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::F_BrokenGlassRendererBehaviour::OnDestroy()
{
}
