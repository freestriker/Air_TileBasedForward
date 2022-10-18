#include "Test/F_WallRendererBehaviour.h"
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
	rttr::registration::class_<AirEngine::Test::F_WallRendererBehaviour>("AirEngine::Test::F_WallRendererBehaviour");
}

AirEngine::Test::F_WallRendererBehaviour::F_WallRendererBehaviour()
{
}

AirEngine::Test::F_WallRendererBehaviour::~F_WallRendererBehaviour()
{
}

void AirEngine::Test::F_WallRendererBehaviour::OnAwake()
{
}

void AirEngine::Test::F_WallRendererBehaviour::OnStart()
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
		auto diffuseTexture2D = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>("..\\Asset\\Texture\\WallDiffuseTexture2D.json");
		auto normalTexture2D = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>("..\\Asset\\Texture\\WallNormalTexture2D.json");
		auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\F_Opaque_Wall_Shader.shader");
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
}

void AirEngine::Test::F_WallRendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::F_WallRendererBehaviour::OnDestroy()
{
}
