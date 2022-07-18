#include "Core/Logic/CoreObject/Thread.h"
#include <QDebug>
#include "Core/IO/Manager/AssetManager.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Asset/Mesh.h"
#include "Asset/Texture2D.h"
#include "Asset/TextureCube.h"
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
#include "Test/BackgroundRendererBehaviour.h"
#include "Test/F_GlassRendererBehaviour.h"
#include "Test/F_MirrorRendererBehaviour.h"
#include "Test/F_OpaqueRendererBehaviour.h"
#include "Test/F_TransparentRendererBehaviour.h"
#include "Test/TBF_TransparentRendererBehaviour.h"
#include "Renderer/Renderer.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/RenderPass/BackgroundRenderPass.h"
#include "Core/Graphic/RenderPass/F_OpaqueRenderPass.h"
#include "Core/Graphic/RenderPass/TBF_OpaqueRenderPass.h"
#include "Core/Graphic/RenderPass/F_TransparentRenderPass.h"
#include "Core/Graphic/RenderPass/TBF_TransparentRenderPass.h"
#include "Core/Graphic/RenderPass/PreZRenderPass.h"
#include "Core/Graphic/RenderPass/TBF_OIT_DepthPeelingBlendRenderPass.h"
#include "Core/Graphic/RenderPass/TBF_OIT_DepthPeelingRenderPass.h"
#include "Test/TBF_OIT_RedBoxBehaviour.h"
#include "Core/Graphic/RenderPass/PresentRenderPass.h"

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
	Utils::Log::Message("Core::Thread::LogicThread iterate " + std::to_string(static_cast<int>(targetComponentType)) + " by dynamic BFS.");
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

	Utils::Log::Message("Core::Thread::LogicThread iterate " + std::to_string(static_cast<int>(targetComponentType)) + " by static BFS.");
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

	Utils::Log::Message("Core::Thread::LogicThread iterate " + std::to_string(static_cast<int>(targetComponentType)) + " by static BFS with record.");
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

	Utils::Log::Message("Core::Thread::LogicThread iterate " + targetComponentTypeString + "by static BFS with record.");
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

	Utils::Log::Message("Core::Thread::LogicThread iterate " + targetComponentTypeString + "by static BFS with record.");
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
	qDebug() << "AirEngine::Core::Logic::CoreObject::Thread::LogicThread::Init()";
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnStart()
{
	_stopped = false;
	Instance::time.Launch();
	qDebug() << "AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnStart()";
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnThreadStart()
{
	qDebug() << "AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnThreadStart()";
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnRun()
{
	qDebug() << "AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnRun()";

	Graphic::CoreObject::Instance::RenderPassManager().AddRenderPass(new Graphic::RenderPass::BackgroundRenderPass());
	Graphic::CoreObject::Instance::RenderPassManager().AddRenderPass(new Graphic::RenderPass::F_OpaqueRenderPass());
	Graphic::CoreObject::Instance::RenderPassManager().AddRenderPass(new Graphic::RenderPass::PreZRenderPass());
	Graphic::CoreObject::Instance::RenderPassManager().AddRenderPass(new Graphic::RenderPass::F_TransparentRenderPass());
	Graphic::CoreObject::Instance::RenderPassManager().AddRenderPass(new Graphic::RenderPass::TBF_OpaqueRenderPass());
	Graphic::CoreObject::Instance::RenderPassManager().AddRenderPass(new Graphic::RenderPass::TBF_TransparentRenderPass());
	//Graphic::CoreObject::Instance::RenderPassManager().AddRenderPass(new Graphic::RenderPass::TBF_OIT_DepthPeelingRenderPass());
	//Graphic::CoreObject::Instance::RenderPassManager().AddRenderPass(new Graphic::RenderPass::TBF_OIT_DepthPeelingBlendRenderPass());
	Graphic::CoreObject::Instance::RenderPassManager().AddRenderPass(new Graphic::RenderPass::PresentRenderPass());

	//Camera
	Object::GameObject* cameraGo = new Logic::Object::GameObject("Camera");
	CoreObject::Instance::rootObject.AddChild(cameraGo);
	auto camera = new Camera::PerspectiveCamera(
		{ 
			"PreZRenderPass",
			"F_OpaqueRenderPass",
			"F_TransparentRenderPass",
			"BackgroundRenderPass", 
			"TBF_OpaqueRenderPass",
			"TBF_TransparentRenderPass",
			//"TBF_OIT_DepthPeelingRenderPass", 
			//"TBF_OIT_DepthPeelingBlendRenderPass"
		},
		{
			{"ColorAttachment", Graphic::Instance::Image::Create2DImage({800, 450}, VK_FORMAT_R8G8B8A8_SRGB, VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT)},
			{"DepthAttachment", Graphic::Instance::Image::Create2DImage({800, 450}, VK_FORMAT_D32_SFLOAT, VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT)}
		}
	);
	camera->mainCamera = camera;
	cameraGo->AddComponent(camera);
	cameraGo->AddComponent(new Test::CameraMoveBehaviour());

	//Renderers
	Logic::Object::GameObject* renderers = new Logic::Object::GameObject("Renderers");
	CoreObject::Instance::rootObject.AddChild(renderers);

	Logic::Object::GameObject* opaqueRendererGo = new Logic::Object::GameObject("meshRenderer");
	renderers->AddChild(opaqueRendererGo);
	opaqueRendererGo->AddComponent(new Renderer::Renderer());
	opaqueRendererGo->AddComponent(new Test::F_OpaqueRendererBehaviour());
	opaqueRendererGo->transform.SetScale(glm::vec3(0.8, 0.8, 0.8));

	Logic::Object::GameObject* glassMeshRendererGo = new Logic::Object::GameObject("GlassMeshRenderer");
	renderers->AddChild(glassMeshRendererGo);
	glassMeshRendererGo->AddComponent(new Renderer::Renderer());
	glassMeshRendererGo->AddComponent(new Test::F_GlassRendererBehaviour());
	glassMeshRendererGo->transform.SetTranslation(glm::vec3(3, 0, 0));

	Logic::Object::GameObject* mirrorMeshRendererGo = new Logic::Object::GameObject("MirrorMeshRenderer");
	renderers->AddChild(mirrorMeshRendererGo);
	mirrorMeshRendererGo->AddComponent(new Renderer::Renderer());
	mirrorMeshRendererGo->AddComponent(new Test::F_MirrorRendererBehaviour());
	mirrorMeshRendererGo->transform.SetTranslation(glm::vec3(-3, 0, 0));

	//Logic::Object::GameObject* culledRendererGo = new Logic::Object::GameObject("MeshRendererCulled");
	//renderers->AddChild(culledRendererGo);
	//culledRendererGo->AddComponent(new Renderer::Renderer());
	//culledRendererGo->AddComponent(new Test::F_OpaqueRendererBehaviour());
	//culledRendererGo->transform.SetTranslation(glm::vec3(2000, 2000, 2000));

	Logic::Object::GameObject* transparentRenderers = new Logic::Object::GameObject("TransparentRenderers");
	renderers->AddChild(transparentRenderers);
	{
		Logic::Object::GameObject* transparentRendererGo = new Logic::Object::GameObject("F_TransparentRenderer1");
		transparentRenderers->AddChild(transparentRendererGo);
		transparentRendererGo->AddComponent(new Renderer::Renderer());
		transparentRendererGo->AddComponent(new Test::F_TransparentRendererBehaviour());
		transparentRendererGo->transform.SetScale(glm::vec3(2, 2, 2));
		transparentRendererGo->transform.SetTranslation(glm::vec3(2, 0, 2));
	}
	{
		Logic::Object::GameObject* transparentRendererGo = new Logic::Object::GameObject("F_TransparentRenderer2");
		transparentRenderers->AddChild(transparentRendererGo);
		transparentRendererGo->AddComponent(new Renderer::Renderer());
		transparentRendererGo->AddComponent(new Test::F_TransparentRendererBehaviour());
		transparentRendererGo->transform.SetScale(glm::vec3(2, 2, 2));
		transparentRendererGo->transform.SetTranslation(glm::vec3(2, 2, 0));
		transparentRendererGo->transform.SetEulerRotation(glm::vec3(90, 0, 0));
	}
	{
		Logic::Object::GameObject* transparentRendererGo = new Logic::Object::GameObject("F_TransparentRenderer3");
		transparentRenderers->AddChild(transparentRendererGo);
		transparentRendererGo->AddComponent(new Renderer::Renderer());
		transparentRendererGo->AddComponent(new Test::F_TransparentRendererBehaviour());
		transparentRendererGo->transform.SetScale(glm::vec3(2, 2, 2));
		transparentRendererGo->transform.SetTranslation(glm::vec3(-2, 2, 0));
		transparentRendererGo->transform.SetEulerRotation(glm::vec3(90, 0, 0));
	}
	{
		Logic::Object::GameObject* transparentRendererGo = new Logic::Object::GameObject("TBF_TransparentRenderer1");
		transparentRenderers->AddChild(transparentRendererGo);
		transparentRendererGo->AddComponent(new Renderer::Renderer());
		transparentRendererGo->AddComponent(new Test::TBF_TransparentRendererBehaviour());
		transparentRendererGo->transform.SetScale(glm::vec3(2, 2, 2));
		transparentRendererGo->transform.SetTranslation(glm::vec3(-2, 0, 2));
	}
	{
		Logic::Object::GameObject* transparentRendererGo = new Logic::Object::GameObject("TBF_TransparentRenderer2");
		transparentRenderers->AddChild(transparentRendererGo);
		transparentRendererGo->AddComponent(new Renderer::Renderer());
		transparentRendererGo->AddComponent(new Test::TBF_TransparentRendererBehaviour());
		transparentRendererGo->transform.SetScale(glm::vec3(2, 2, 2));
		transparentRendererGo->transform.SetTranslation(glm::vec3(4, 0, 0));
		transparentRendererGo->transform.SetEulerRotation(glm::vec3(90, 90, 90));
	}

	///OIT
	Logic::Object::GameObject* oitRenderers = new Logic::Object::GameObject("OitRenderers");
	renderers->AddChild(oitRenderers);
	//{
	//	Logic::Object::GameObject* oitRendererGo = new Logic::Object::GameObject("OitRenderer1");
	//	oitRenderers->AddChild(oitRendererGo);
	//	oitRendererGo->AddComponent(new Renderer::Renderer());
	//	oitRendererGo->transform.SetScale(glm::vec3(1, 1, 1));
	//	oitRendererGo->AddComponent(new Test::TBF_OIT_RedBoxBehaviour());
	//	oitRendererGo->AddComponent(new Test::SelfRotateBehaviour(30));
	//}
	//{
	//	Logic::Object::GameObject* oitRendererGo = new Logic::Object::GameObject("OitRenderer2");
	//	oitRenderers->AddChild(oitRendererGo);
	//	oitRendererGo->AddComponent(new Renderer::Renderer());
	//	oitRendererGo->transform.SetScale(glm::vec3(1.5, 1.5, 1.5));
	//	oitRendererGo->AddComponent(new Test::TBF_OIT_RedBoxBehaviour());
	//	oitRendererGo->AddComponent(new Test::SelfRotateBehaviour(60));
	//}

	//Lights
	Logic::Object::GameObject* lights = new Logic::Object::GameObject("Lights");
	CoreObject::Instance::rootObject.AddChild(lights);

	Logic::Object::GameObject* directionalLightGo = new Logic::Object::GameObject("DirectionalLight");
	lights->AddChild(directionalLightGo);
	directionalLightGo->transform.SetEulerRotation(glm::vec3(-30, 70, 0));
	auto directionalLight = new Light::DirectionalLight();
	directionalLight->color = { 1, 239.0 / 255, 213.0 / 255, 1 };
	directionalLight->intensity = 0.8f;
	directionalLightGo->AddComponent(directionalLight);

	Logic::Object::GameObject* skyBoxGo = new Logic::Object::GameObject("SkyBox");
	lights->AddChild(skyBoxGo);
	auto skyBox = new Light::AmbientLight();
	skyBox->color = { 1, 1, 1, 1 };
	skyBox->intensity = 0.8f;
	skyBox->ambientLightTextureCube = IO::CoreObject::Instance::AssetManager().Load<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
	skyBoxGo->AddComponent(skyBox);

	float sr6 = std::pow(6.0f, 0.5f);
	float sr2 = std::pow(2.0f, 0.5f);

	Logic::Object::GameObject* nearPointLights = new Logic::Object::GameObject("NearPointLights");
	lights->AddChild(nearPointLights);
	nearPointLights->AddComponent(new Test::SelfRotateBehaviour(60));
	{
		Logic::Object::GameObject* pointLightGo = new Logic::Object::GameObject("NearPointLight 1");
		nearPointLights->AddChild(pointLightGo);
		pointLightGo->transform.SetTranslation({ 0, 0, 2 });
		auto pointLight = new Light::PointLight();
		pointLight->color = { 1, 1, 0, 1 };
		pointLight->minRange = 0.01;
		pointLight->maxRange = 4;
		pointLightGo->AddComponent(pointLight);
	}
	{
		Logic::Object::GameObject* pointLightGo = new Logic::Object::GameObject("NearPointLight 2");
		nearPointLights->AddChild(pointLightGo);
		pointLightGo->transform.SetTranslation({ -sr6 * 2 / 3, -sr2 * 2 / 3, -2.0 / 3 });
		auto pointLight = new Light::PointLight();
		pointLight->color = { 1, 0, 0, 1 };
		pointLight->minRange = 0.01;
		pointLight->maxRange = 4;
		pointLightGo->AddComponent(pointLight);
	}
	{
		Logic::Object::GameObject* pointLightGo = new Logic::Object::GameObject("NearPointLight 3");
		nearPointLights->AddChild(pointLightGo);
		pointLightGo->transform.SetTranslation({ sr6 * 2 / 3, -sr2 * 2 / 3, -2.0 / 3 });
		auto pointLight = new Light::PointLight();
		pointLight->color = { 0, 1, 0, 1 };
		pointLight->minRange = 0.01;
		pointLight->maxRange = 4;
		pointLightGo->AddComponent(pointLight);
	}
	{
		Logic::Object::GameObject* pointLightGo = new Logic::Object::GameObject("NearPointLight 4");
		nearPointLights->AddChild(pointLightGo);
		pointLightGo->transform.SetTranslation({ 0, sr2 * 2 * 2 / 3, -2.0 / 3 });
		auto pointLight = new Light::PointLight();
		pointLight->color = { 0, 0, 1, 1 };
		pointLight->minRange = 0.01;
		pointLight->maxRange = 4;
		pointLightGo->AddComponent(pointLight);
	}

	while (!_stopped)
	{
		Instance::time.Refresh();

		IterateByDynamicBfs(Object::Component::ComponentType::BEHAVIOUR);
		auto cameras = std::vector<Object::Component*>();

		if (Instance::NeedIterateRenderer())
		{
			//Iterate renderer
			Utils::Log::Message("Iterate renderer");
			auto targetComponents = std::vector<std::vector<Object::Component*>>();
			IterateByStaticBfs({ Object::Component::ComponentType::LIGHT, Object::Component::ComponentType::CAMERA, Object::Component::ComponentType::RENDERER }, targetComponents);

			Graphic::CoreObject::Instance::AddLight(targetComponents[0]);
			Graphic::CoreObject::Instance::AddCamera(targetComponents[1]);
			Graphic::CoreObject::Instance::AddRenderer(targetComponents[2]);

			Utils::Log::Message("Instance::StartRenderCondition().Awake()");
			Graphic::CoreObject::Instance::StartRenderCondition().Awake();
			Graphic::CoreObject::Instance::EndRenderCondition().Wait();
			Utils::Log::Message("Instance::EndRenderCondition().Wait()");

			Graphic::CoreObject::Instance::ClearLight();
			Graphic::CoreObject::Instance::ClearCamera();
			Graphic::CoreObject::Instance::ClearRenderer();
		}
		std::this_thread::yield();
	}
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnEnd()
{
	qDebug() << "AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnEnd()";
}
