#include "Test/TBF_Opaque_Pbr_RendererBehaviour.h"
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
	rttr::registration::class_<AirEngine::Test::TBF_Opaque_Pbr_RendererBehaviour>("AirEngine::Test::TBF_Opaque_Pbr_RendererBehaviour");
}

AirEngine::Test::TBF_Opaque_Pbr_RendererBehaviour::TBF_Opaque_Pbr_RendererBehaviour()
	: TBF_Opaque_Pbr_RendererBehaviour("..\\Asset\\Mesh\\Sphere.ply", "..\\Asset\\Texture\\MetalFloor")
{
}

AirEngine::Test::TBF_Opaque_Pbr_RendererBehaviour::~TBF_Opaque_Pbr_RendererBehaviour()
{
}

AirEngine::Test::TBF_Opaque_Pbr_RendererBehaviour::TBF_Opaque_Pbr_RendererBehaviour(std::string meshPath, std::string textureBasePath)
	: _meshPath(meshPath)
	, _textureBasePath(textureBasePath)
{
}

void AirEngine::Test::TBF_Opaque_Pbr_RendererBehaviour::OnAwake()
{
}

void AirEngine::Test::TBF_Opaque_Pbr_RendererBehaviour::OnStart()
{
	auto renderer = new Renderer::Renderer();
	renderer->mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>(_meshPath);
	GameObject()->AddComponent(renderer);

	auto sampler = new Core::Graphic::Instance::ImageSampler(
		VkFilter::VK_FILTER_LINEAR,
		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR,
		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
		0.0f,
		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	);

	{
		auto albedoTexture = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>(_textureBasePath + "_Albedo.json");
		auto normalTexture = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>(_textureBasePath + "_Normal.json");
		auto rmoTexture = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>(_textureBasePath + "_Rmo.json");
		auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\TBF_Opaque_Pbr_Shader.shader");
		auto material = new Core::Graphic::Rendering::Material(shader);
		material->SetSampledImage2D("albedoTexture", albedoTexture, sampler);
		material->SetSampledImage2D("normalTexture", normalTexture, sampler);
		material->SetSampledImage2D("rmoTexture", rmoTexture, sampler);
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

void AirEngine::Test::TBF_Opaque_Pbr_RendererBehaviour::OnUpdate()
{

}

void AirEngine::Test::TBF_Opaque_Pbr_RendererBehaviour::OnDestroy()
{
}
