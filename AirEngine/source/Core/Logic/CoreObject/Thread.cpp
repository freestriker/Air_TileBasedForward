#include "Core/Logic/CoreObject/Thread.h"
#include <QDebug>
#include "Core/IO/Manager/AssetManager.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Asset/Mesh.h"
#include "Utils/Log.h"
#include "Utils/Condition.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Camera/PerspectiveCamera.h"
#include "Test/CameraMoveBehaviour.h"
#include "Core/Graphic/Instance/Image.h"
#include "Light/DirectionalLight.h"
#include "Light/PointLight.h"
#include "Light/AmbientLight.h"
#include "Test/SelfRotateBehaviour.h"
#include "Test/Background_SkyboxRendererBehaviour.h"
#include "Test/F_GlassRendererBehaviour.h"
#include "Test/F_MirrorRendererBehaviour.h"
#include "Test/F_OpaqueRendererBehaviour.h"
#include "Test/F_BrokenGlassRendererBehaviour.h"
#include "Renderer/Renderer.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Test/TBF_OIT_RenderBehaviour.h"
#include "Asset/AudioClip.h"
#include "Audio/AudioSource.h"
#include "Audio/AudioListener.h"
#include "Test/AudioSourceBehaviour.h"
#include "Core/Logic/Manager/InputManager.h"
#include "Rendering/RenderPipeline/BaseRenderPipeline.h"
#include "Core/Graphic/Manager/RenderPipelineManager.h"
#include "Test/F_WallRendererBehaviour.h"
#include "Rendering/Renderer/TBForwardRenderer.h"
#include "Test/TBF_WallRendererBehaviour.h"
#include "Test/TBF_GlassRendererBehaviour.h"
#include "Test/TBF_MirrorRendererBehaviour.h"
#include "Test/TBF_OIT_RenderBehaviour.h"
#include "Test/QuadMoveBehaviour.h"
#include "Test/RendererDataController.h"
#include "Test/TBF_Opaque_Pbr_RendererBehaviour.h"
#include "Test/TBF_Opaque_Pbr_Mirror_RendererBehaviour.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Test/IfftOcean_WindRotationBehaviour.h"

AirEngine::Core::Logic::CoreObject::Thread::LogicThread AirEngine::Core::Logic::CoreObject::Thread::_logicThread = AirEngine::Core::Logic::CoreObject::Thread::LogicThread();

AirEngine::Core::Logic::CoreObject::Thread::Thread()
{
}

AirEngine::Core::Logic::CoreObject::Thread::~Thread()
{
}

void AirEngine::Core::Logic::CoreObject::Thread::Init()
{
	_logicThread.Init();
}

void AirEngine::Core::Logic::CoreObject::Thread::Start()
{
	_logicThread.Start();
}

void AirEngine::Core::Logic::CoreObject::Thread::End()
{
	_logicThread.End();
}

void AirEngine::Core::Logic::CoreObject::Thread::WaitForStartFinish()
{
	_logicThread.WaitForStartFinish();
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::IterateByDynamicBfs(Object::Component::ComponentType targetComponentType)
{
	std::list< Logic::Object::GameObject*> curGenGameObjectHeads = std::list<Logic::Object::GameObject*>();
	std::list< Logic::Object::GameObject*> nextGenGameObjectHeads = std::list<Logic::Object::GameObject*>();
	auto& validGameObjectInIteration = Instance::_validGameObjectInIteration;
	auto& validComponentInIteration = Instance::_validComponentInIteration;

	//Clear
	validGameObjectInIteration.clear();
	validComponentInIteration.clear();
	curGenGameObjectHeads.clear();
	nextGenGameObjectHeads.clear();

	//Init
	if (Instance::rootObject._gameObject.Child())
	{
		validGameObjectInIteration.emplace(Instance::rootObject._gameObject.Child());

		curGenGameObjectHeads.emplace_back(Instance::rootObject._gameObject.Child());
	}

	while (!curGenGameObjectHeads.empty())
	{
		for (const auto& curGenGameObjectHead : curGenGameObjectHeads)
		{
			if (!validGameObjectInIteration.count(curGenGameObjectHead)) continue;

			std::vector<Logic::Object::GameObject*> curGenGameObjects = std::vector<Logic::Object::GameObject*>();
			{
				curGenGameObjects.emplace_back(curGenGameObjectHead);
				Object::GameObject* gameObject = curGenGameObjectHead->Brother();
				while (gameObject)
				{
					curGenGameObjects.emplace_back(gameObject);
					validGameObjectInIteration.emplace(gameObject);

					gameObject = gameObject->Brother();
				}
			}

			for (const auto& curGenGameObject : curGenGameObjects)
			{
				if (!validGameObjectInIteration.count(curGenGameObject))continue;

				//Run Components
				validComponentInIteration.clear();
				if (curGenGameObject->_typeSqueueComponentsHeadMap.count(targetComponentType))
				{
					std::vector< Object::Component*> components = std::vector< Object::Component*>();
					for (auto iterator = curGenGameObject->_typeSqueueComponentsHeadMap[targetComponentType]->GetIterator(); iterator.IsValid(); iterator++)
					{
						auto component = static_cast<Object::Component*>(iterator.Node());

						validComponentInIteration.insert(component);
						components.emplace_back(component);
					}

					for (const auto& component : components)
					{
						if (validComponentInIteration.count(component)) component->Update();
					}
				}

				if (!validGameObjectInIteration.count(curGenGameObject))continue;
				if (curGenGameObject->Child())
				{
					auto childHead = curGenGameObject->Child();

					nextGenGameObjectHeads.emplace_back(childHead);
					validGameObjectInIteration.emplace(childHead);
				}
			}
		}

		curGenGameObjectHeads.clear();
		std::swap(nextGenGameObjectHeads, curGenGameObjectHeads);
	}

	validGameObjectInIteration.clear();
	validComponentInIteration.clear();
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::IterateByStaticBfs(Object::Component::ComponentType targetComponentType)
{
	std::list< Logic::Object::GameObject*> curGenGameObjectHeads = std::list<Logic::Object::GameObject*>();
	std::list< Logic::Object::GameObject*> nextGenGameObjectHeads = std::list<Logic::Object::GameObject*>();

	//Init
	if (Instance::rootObject._gameObject.Child())
	{
		curGenGameObjectHeads.emplace_back(Instance::rootObject._gameObject.Child());
	}

	while (!curGenGameObjectHeads.empty())
	{
		for (const auto& curGenGameObjectHead : curGenGameObjectHeads)
		{
			Object::GameObject* curGenGameObject = curGenGameObjectHead;
			while (curGenGameObject)
			{
				//Update components
				if (curGenGameObject->_typeSqueueComponentsHeadMap.count(targetComponentType))
				{
					for (auto iterator = curGenGameObject->_typeSqueueComponentsHeadMap[targetComponentType]->GetIterator(); iterator.IsValid(); iterator++)
					{
						auto component = static_cast<Object::Component*>(iterator.Node());

						component->Update();
					}
				}

				//Add next gen GameObject
				if (curGenGameObject->Child())
				{
					auto childHead = curGenGameObject->Child();

					nextGenGameObjectHeads.emplace_back(childHead);
				}

				curGenGameObject = curGenGameObject->Brother();
			}

		}

		curGenGameObjectHeads.clear();
		std::swap(nextGenGameObjectHeads, curGenGameObjectHeads);
	}
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::IterateByStaticBfs(Object::Component::ComponentType targetComponentType, std::vector<Object::Component*>& targetComponents)
{
	std::list< Logic::Object::GameObject*> curGenGameObjectHeads = std::list<Logic::Object::GameObject*>();
	std::list< Logic::Object::GameObject*> nextGenGameObjectHeads = std::list<Logic::Object::GameObject*>();

	//Init
	if (Instance::rootObject._gameObject.Child())
	{
		curGenGameObjectHeads.emplace_back(Instance::rootObject._gameObject.Child());
	}

	while (!curGenGameObjectHeads.empty())
	{
		for (const auto& curGenGameObjectHead : curGenGameObjectHeads)
		{
			Object::GameObject* curGenGameObject = curGenGameObjectHead;
			while (curGenGameObject)
			{
				//Update components
				if (curGenGameObject->_typeSqueueComponentsHeadMap.count(targetComponentType))
				{
					for (auto iterator = curGenGameObject->_typeSqueueComponentsHeadMap[targetComponentType]->GetIterator(); iterator.IsValid(); iterator++)
					{
						auto component = static_cast<Object::Component*>(iterator.Node());

						component->Update();

						targetComponents.emplace_back(component);
					}
				}

				//Add next gen GameObject
				if (curGenGameObject->Child())
				{
					auto childHead = curGenGameObject->Child();

					nextGenGameObjectHeads.emplace_back(childHead);
				}

				curGenGameObject = curGenGameObject->Brother();
			}

		}

		curGenGameObjectHeads.clear();
		std::swap(nextGenGameObjectHeads, curGenGameObjectHeads);
	}
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::IterateByStaticBfs(std::vector<Object::Component::ComponentType> targetComponentTypes)
{
	std::string targetComponentTypeString = "";
	for (const auto& type : targetComponentTypes)
	{
		targetComponentTypeString += std::to_string(static_cast<int>(type)) + " ";
	}

	std::list< Logic::Object::GameObject*> curGenGameObjectHeads = std::list<Logic::Object::GameObject*>();
	std::list< Logic::Object::GameObject*> nextGenGameObjectHeads = std::list<Logic::Object::GameObject*>();

	//Init
	if (Instance::rootObject._gameObject.Child())
	{
		curGenGameObjectHeads.emplace_back(Instance::rootObject._gameObject.Child());
	}

	while (!curGenGameObjectHeads.empty())
	{
		for (const auto& curGenGameObjectHead : curGenGameObjectHeads)
		{
			Object::GameObject* curGenGameObject = curGenGameObjectHead;
			while (curGenGameObject)
			{
				//Update components
				for (uint32_t i = 0; i < targetComponentTypes.size(); i++)
				{
					if (curGenGameObject->_typeSqueueComponentsHeadMap.count(targetComponentTypes[i]))
					{
						for (auto iterator = curGenGameObject->_typeSqueueComponentsHeadMap[targetComponentTypes[i]]->GetIterator(); iterator.IsValid(); iterator++)
						{
							auto component = static_cast<Object::Component*>(iterator.Node());

							component->Update();

						}
					}
				}

				//Add next gen GameObject
				if (curGenGameObject->Child())
				{
					auto childHead = curGenGameObject->Child();

					nextGenGameObjectHeads.emplace_back(childHead);
				}

				curGenGameObject = curGenGameObject->Brother();
			}

		}

		curGenGameObjectHeads.clear();
		std::swap(nextGenGameObjectHeads, curGenGameObjectHeads);
	}
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::IterateByStaticBfs(std::vector<Object::Component::ComponentType> targetComponentTypes, std::vector<std::vector<Object::Component*>>& targetComponents)
{
	std::string targetComponentTypeString = "";
	for (const auto& type : targetComponentTypes)
	{
		targetComponentTypeString += std::to_string(static_cast<int>(type)) + " ";
	}

	std::list< Logic::Object::GameObject*> curGenGameObjectHeads = std::list<Logic::Object::GameObject*>();
	std::list< Logic::Object::GameObject*> nextGenGameObjectHeads = std::list<Logic::Object::GameObject*>();
	targetComponents.clear();
	targetComponents.resize(targetComponentTypes.size());

	//Init
	if (Instance::rootObject._gameObject.Child())
	{
		curGenGameObjectHeads.emplace_back(Instance::rootObject._gameObject.Child());
	}

	while (!curGenGameObjectHeads.empty())
	{
		for (const auto& curGenGameObjectHead : curGenGameObjectHeads)
		{
			Object::GameObject* curGenGameObject = curGenGameObjectHead;
			while (curGenGameObject)
			{
				//Update components
				for (uint32_t i = 0; i < targetComponentTypes.size(); i++)
				{
					if (curGenGameObject->_typeSqueueComponentsHeadMap.count(targetComponentTypes[i]))
					{
						for (auto iterator = curGenGameObject->_typeSqueueComponentsHeadMap[targetComponentTypes[i]]->GetIterator(); iterator.IsValid(); iterator++)
						{
							auto component = static_cast<Object::Component*>(iterator.Node());

							component->Update();

							targetComponents[i].emplace_back(component);
						}
					}
				}

				//Add next gen GameObject
				if (curGenGameObject->Child())
				{
					auto childHead = curGenGameObject->Child();

					nextGenGameObjectHeads.emplace_back(childHead);
				}

				curGenGameObject = curGenGameObject->Brother();
			}

		}

		curGenGameObjectHeads.clear();
		std::swap(nextGenGameObjectHeads, curGenGameObjectHeads);
	}
}

AirEngine::Core::Logic::CoreObject::Thread::LogicThread::LogicThread()
	: _stopped(true)
{
}

AirEngine::Core::Logic::CoreObject::Thread::LogicThread::~LogicThread()
{
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::Init()
{
	_stopped = false;
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnStart()
{
	_stopped = false;
	Instance::Init();
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnThreadStart()
{

}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnRun()
{
	Graphic::CoreObject::Instance::RenderPipelineManager().SwitchRenderPipeline(new Rendering::RenderPipeline::BaseRenderPipeline());
	
	//Camera
	Object::GameObject* cameraGo = new Logic::Object::GameObject("Camera");
	CoreObject::Instance::rootObject.AddChild(cameraGo);
	auto camera = new Camera::PerspectiveCamera(
		"TBForwardRenderer",
		//"ForwardRenderer",
		{
			{"ColorAttachment", Graphic::Instance::Image::Create2DImage({1600, 900}, VK_FORMAT_R8G8B8A8_SRGB, VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT)},
			{"DepthAttachment", Graphic::Instance::Image::Create2DImage({1600, 900}, VK_FORMAT_D32_SFLOAT, VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT)}
		}
	);
	camera->fovAngle = 60;
	camera->mainCamera = camera;
	cameraGo->AddComponent(camera);
	cameraGo->AddComponent(new Test::CameraMoveBehaviour());
	cameraGo->AddComponent(new Audio::AudioListener());
	cameraGo->AddComponent(new Test::RendererDataController());
	cameraGo->transform.SetTranslation({ 0, 0.5, 5 });

	///AudioSource
	Logic::Object::GameObject* audioSources = new Logic::Object::GameObject("AudioSources");
	CoreObject::Instance::rootObject.AddChild(audioSources);
	{
		Logic::Object::GameObject* audioSourceGo = new Logic::Object::GameObject("AudioSource");
		audioSources->AddChild(audioSourceGo);
		audioSourceGo->AddComponent(new Audio::AudioSource());
		audioSourceGo->AddComponent(new Test::AudioSourceBehaviour("..\\Asset\\Audio\\IWouldGiveYouAnything.flac"));
		audioSourceGo->transform.SetScale(glm::vec3(0.8, 0.8, 0.8));
	}

	//Renderers
	Logic::Object::GameObject* renderers = new Logic::Object::GameObject("Renderers");
	CoreObject::Instance::rootObject.AddChild(renderers);

	///Background
	{
		Logic::Object::GameObject* backgroundRendererGo = new Logic::Object::GameObject("BackgroundRenderer");
		renderers->AddChild(backgroundRendererGo);
		backgroundRendererGo->AddComponent(new Renderer::Renderer());
		backgroundRendererGo->AddComponent(new Test::Background_SkyboxRendererBehaviour());
	}

	/////TBForward opaque
	//{
	//	Logic::Object::GameObject* pbrRendererGo = new Logic::Object::GameObject("PbrRenderer");
	//	renderers->AddChild(pbrRendererGo);
	//	pbrRendererGo->AddComponent(new Test::TBF_Opaque_Pbr_RendererBehaviour());
	//	pbrRendererGo->AddComponent(new Test::SelfRotateBehaviour(60));
	//	pbrRendererGo->transform.SetScale(glm::vec3(0.8, 0.8, 0.8));

	//	Logic::Object::GameObject* mirrorMeshRendererGo = new Logic::Object::GameObject("MirrorRenderer");
	//	renderers->AddChild(mirrorMeshRendererGo);
	//	mirrorMeshRendererGo->AddComponent(new Renderer::Renderer());
	//	mirrorMeshRendererGo->AddComponent(new Test::TBF_Opaque_Pbr_Mirror_RendererBehaviour());
	//	mirrorMeshRendererGo->AddComponent(new Test::SelfRotateBehaviour(45));
	//	mirrorMeshRendererGo->transform.SetTranslation(glm::vec3(-3, 0, 0));

	//	for (int i = -4; i <= 4; i++)
	//	{
	//		for (int j = -4; j <= 4; j++)
	//		{
	//			if (i == 0 && j == 0) continue;
	//			Logic::Object::GameObject* sphereGroupRendererGo = new Logic::Object::GameObject("SphereGroup_" + std::to_string(i) + " " + std::to_string(j));
	//			renderers->AddChild(sphereGroupRendererGo);
	//			sphereGroupRendererGo->AddComponent(new Test::TBF_Opaque_Pbr_RendererBehaviour("..\\Asset\\Mesh\\Sphere.ply", "..\\Asset\\Texture\\MetalFloor"));
	//			sphereGroupRendererGo->transform.SetTranslation(glm::vec3(i * 10, 0, j * 10));
	//		}
	//	}
	//}

	/////TBForward quad mover
	//{
	//	for (int i = -2; i <= 2; i++)
	//	{
	//		for (int j = -2; j <= 2; j++)
	//		{
	//			Logic::Object::GameObject* quadRendererGo = new Logic::Object::GameObject("QuadRenderer_" + std::to_string(i) + " " + std::to_string(j));
	//			renderers->AddChild(quadRendererGo);
	//			quadRendererGo->AddComponent(new Test::TBF_Opaque_Pbr_RendererBehaviour("..\\Asset\\Mesh\\LargeQuad.ply", "..\\Asset\\Texture\\Bricks"));
	//			quadRendererGo->AddComponent(new Test::QuadMoveBehaviour());
	//			quadRendererGo->transform.SetTranslation(glm::vec3(i * 20, -1, j * 20));
	//			quadRendererGo->transform.SetScale({ 10, 1, 10 });
	//		}
	//	}
	//}

	//{
	//	Logic::Object::GameObject* surfaceRendererGo = new Logic::Object::GameObject("SurfaceRenderer");
	//	renderers->AddChild(surfaceRendererGo);

	//	auto&& renderer = new Renderer::Renderer();

	//	surfaceRendererGo->AddComponent(renderer);
	//	surfaceRendererGo->transform.SetTranslation({ 0, -1, -1 });
	//	surfaceRendererGo->transform.SetScale({ 10, 1, 10 });

	//	renderer->mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Surface.ply");
	//	surfaceRendererGo->AddComponent(renderer);

	//	auto sampler = new Core::Graphic::Instance::ImageSampler(
	//		VkFilter::VK_FILTER_LINEAR,
	//		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR,
	//		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
	//		0.0f,
	//		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	//	);

	//	{
	//		std::string _textureBasePath = "..\\Asset\\Texture\\Bricks";
	//		auto albedoTexture = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>(_textureBasePath + "_Albedo.json");
	//		//auto normalTexture = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>(_textureBasePath + "_Normal.json");
	//		//auto rmoTexture = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>(_textureBasePath + "_Rmo.json");
	//		auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\IWave_Surface_Shader.shader");
	//		auto material = new Core::Graphic::Rendering::Material(shader);
	//		material->SetSampledImage2D("albedoTexture", albedoTexture, sampler);
	//		//material->SetSampledImage2D("normalTexture", normalTexture, sampler);
	//		//material->SetSampledImage2D("rmoTexture", rmoTexture, sampler);
	//		renderer->AddMaterial(material);
	//	}
	//	//{
	//	//	auto geometryShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\Geometry_Shader.shader");
	//	//	auto geometryMaterial = new Core::Graphic::Rendering::Material(geometryShader);
	//	//	renderer->AddMaterial(geometryMaterial);
	//	//}
	//}

	//{
	//	Logic::Object::GameObject* surfaceRendererGo = new Logic::Object::GameObject("FftOceanRenderer");
	//	renderers->AddChild(surfaceRendererGo);

	//	//auto&& windRotator = new Test::IfftOcean_WindRotationBehaviour();
	//	//windRotator->windRotationAngleSpeed = 35;
	//	//surfaceRendererGo->AddComponent(windRotator);

	//	auto&& renderer = new Renderer::Renderer();

	//	surfaceRendererGo->AddComponent(renderer);
	//	surfaceRendererGo->transform.SetTranslation({ 0, -1, -1 });
	//	surfaceRendererGo->transform.SetScale({ 5, 5, 5 });

	//	renderer->mesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Surface.ply");
	//	surfaceRendererGo->AddComponent(renderer);

	//	auto sampler = new Core::Graphic::Instance::ImageSampler(
	//		VkFilter::VK_FILTER_LINEAR,
	//		VkSamplerMipmapMode::VK_SAMPLER_MIPMAP_MODE_LINEAR,
	//		VkSamplerAddressMode::VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
	//		0.0f,
	//		VkBorderColor::VK_BORDER_COLOR_INT_OPAQUE_BLACK
	//	);

	//	{
	//		//std::string _textureBasePath = "..\\Asset\\Texture\\Bricks";
	//		//auto albedoTexture = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>(_textureBasePath + "_Albedo.json");
	//		//auto normalTexture = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>(_textureBasePath + "_Normal.json");
	//		//auto rmoTexture = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>(_textureBasePath + "_Rmo.json");
	//		auto shader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\FftOcean_Surface_Shader.shader");
	//		auto material = new Core::Graphic::Rendering::Material(shader);
	//		//material->SetSampledImage2D("albedoTexture", albedoTexture, sampler);
	//		//material->SetSampledImage2D("normalTexture", normalTexture, sampler);
	//		//material->SetSampledImage2D("rmoTexture", rmoTexture, sampler);
	//		renderer->AddMaterial(material);
	//	}
	//	//{
	//	//	auto geometryShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\Geometry_Shader.shader");
	//	//	auto geometryMaterial = new Core::Graphic::Rendering::Material(geometryShader);
	//	//	renderer->AddMaterial(geometryMaterial);
	//	//}
	//}

	/////OIT
	//Logic::Object::GameObject* oitRenderers = new Logic::Object::GameObject("OitRenderers");
	//renderers->AddChild(oitRenderers);
	//oitRenderers->transform.SetTranslation({3, 0, 0});
	//{
	//	Logic::Object::GameObject* oitRendererGo = new Logic::Object::GameObject("OitRenderer");
	//	oitRenderers->AddChild(oitRendererGo);
	//	oitRendererGo->AddComponent(new Renderer::Renderer());
	//	oitRendererGo->AddComponent(new Test::TBF_OIT_RenderBehaviour("WhiteTexture2D"));
	//	oitRendererGo->AddComponent(new Test::SelfRotateBehaviour(35));
	//}

	//Lights
	Logic::Object::GameObject* lights = new Logic::Object::GameObject("Lights");
	CoreObject::Instance::rootObject.AddChild(lights);

	Logic::Object::GameObject* directionalLightGo = new Logic::Object::GameObject("DirectionalLight");
	lights->AddChild(directionalLightGo);
	directionalLightGo->transform.SetEulerRotation(glm::vec3(-30, 0, 0));
	auto directionalLight = new Light::DirectionalLight();
	directionalLight->color = { 1, 239.0 / 255, 213.0 / 255, 1 };
	directionalLight->intensity = 5;
	directionalLightGo->AddComponent(directionalLight);

	Logic::Object::GameObject* iblGo = new Logic::Object::GameObject("SkyBox");
	lights->AddChild(iblGo);
	auto iblLight = new Light::AmbientLight();
	iblLight->color = { 1, 1, 1, 1 };
	iblLight->intensity = 1;
	iblLight->_irradianceCubeImage = IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>("..\\Asset\\Texture\\WorkShop_IrradianceMap_Exr_CubeImage.json");
	iblLight->_prefilteredCubeImage = IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>("..\\Asset\\Texture\\WorkShop_PrefilteredMap_Exr_CubeImage.json");
	iblLight->_lutImage = IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Instance::Image>("..\\Asset\\Texture\\LutImage.json");
	iblGo->AddComponent(iblLight);

	float sr6 = std::pow(6.0f, 0.5f);
	float sr2 = std::pow(2.0f, 0.5f);

	Logic::Object::GameObject* nearPointLights = new Logic::Object::GameObject("NearPointLights");
	lights->AddChild(nearPointLights);
	nearPointLights->AddComponent(new Test::SelfRotateBehaviour(60));
	{
		Logic::Object::GameObject* pointLightGo = new Logic::Object::GameObject("NearPointLight 1");
		nearPointLights->AddChild(pointLightGo);
		pointLightGo->transform.SetTranslation({ 0, 2, 0 });
		auto pointLight = new Light::PointLight();
		pointLight->color = { 1, 1, 0, 1 };
		pointLight->minRange = 0.01;
		pointLight->maxRange = 3;
		pointLight->intensity = 7;
		pointLightGo->AddComponent(pointLight);
	}
	{
		Logic::Object::GameObject* pointLightGo = new Logic::Object::GameObject("NearPointLight 2");
		nearPointLights->AddChild(pointLightGo);
		pointLightGo->transform.SetTranslation({ -sr6 * 2 / 3, -2.0 / 3, -sr2 * 2 / 3 });
		auto pointLight = new Light::PointLight();
		pointLight->color = { 1, 0, 0, 1 };
		pointLight->minRange = 0.01;
		pointLight->maxRange = 3;
		pointLight->intensity = 7;
		pointLightGo->AddComponent(pointLight);
	}
	{
		Logic::Object::GameObject* pointLightGo = new Logic::Object::GameObject("NearPointLight 3");
		nearPointLights->AddChild(pointLightGo);
		pointLightGo->transform.SetTranslation({ sr6 * 2 / 3, -2.0 / 3, -sr2 * 2 / 3 });
		auto pointLight = new Light::PointLight();
		pointLight->color = { 0, 1, 0, 1 };
		pointLight->minRange = 0.01;
		pointLight->maxRange = 3;
		pointLight->intensity = 7;
		pointLightGo->AddComponent(pointLight);
	}
	{
		Logic::Object::GameObject* pointLightGo = new Logic::Object::GameObject("NearPointLight 4");
		nearPointLights->AddChild(pointLightGo);
		pointLightGo->transform.SetTranslation({ 0, -2.0 / 3, sr2 * 2 * 2 / 3 });
		auto pointLight = new Light::PointLight();
		pointLight->color = { 0, 0, 1, 1 };
		pointLight->minRange = 0.01;
		pointLight->maxRange = 3;
		pointLight->intensity = 7;
		pointLightGo->AddComponent(pointLight);
	}

	while (!_stopped)
	{
		Instance::time.Refresh();

		Instance::InputManager().Clear();
		Instance::InputManager().Refresh();

		IterateByDynamicBfs(Object::Component::ComponentType::BEHAVIOUR);
		auto cameras = std::vector<Object::Component*>();

		bool needIterateRenderer = Instance::NeedIterateRenderer();
		if (needIterateRenderer)
		{
			//Iterate renderer
			auto targetComponents = std::vector<std::vector<Object::Component*>>();
			IterateByStaticBfs({ Object::Component::ComponentType::LIGHT, Object::Component::ComponentType::CAMERA, Object::Component::ComponentType::RENDERER }, targetComponents);

			Graphic::CoreObject::Instance::AddLight(targetComponents[0]);
			Graphic::CoreObject::Instance::AddCamera(targetComponents[1]);
			Graphic::CoreObject::Instance::AddRenderer(targetComponents[2]);

			Graphic::CoreObject::Instance::StartRenderCondition().Awake();
		}

		IterateByStaticBfs({ Object::Component::ComponentType::AUDIO_SOURCE, Object::Component::ComponentType::AUDIO_LISTENER });

		if (needIterateRenderer)
		{
			//wait render finish
			Graphic::CoreObject::Instance::EndRenderCondition().Wait();

			Graphic::CoreObject::Instance::ClearLight();
			Graphic::CoreObject::Instance::ClearCamera();
			Graphic::CoreObject::Instance::ClearRenderer();
		}

		std::this_thread::yield();
	}
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnEnd()
{

}
