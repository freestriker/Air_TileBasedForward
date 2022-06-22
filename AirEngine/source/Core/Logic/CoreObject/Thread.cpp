#include "Core/Logic/CoreObject/Thread.h"
#include <QDebug>
#include "Core/IO/Manager/AssetManager.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Asset/Mesh.h"
#include "Asset/Texture2D.h"
#include "Asset/TextureCube.h"

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
	auto meshTask = IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\DefaultMesh.ply");

	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Texture2D>("..\\Asset\\Texture\\DefaultTexture2D.json");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Texture2D>("..\\Asset\\Texture\\DefaultTexture2D.json");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Texture2D>("..\\Asset\\Texture\\DefaultTexture2D.json");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::Texture2D>("..\\Asset\\Texture\\DefaultTexture2D.json");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\DefaultTexture2D.json");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\DefaultTexture2D.json");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\DefaultTexture2D.json");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\DefaultTexture2D.json");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\DefaultTexture2D.json");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\DefaultTexture2D.json");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\DefaultTexture2D.json");
	IO::CoreObject::Instance::AssetManager().Load<Asset::Texture2D>("..\\Asset\\Texture\\DefaultTexture2D.json");
	
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
	IO::CoreObject::Instance::AssetManager().LoadAsync<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
	IO::CoreObject::Instance::AssetManager().Load<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
	IO::CoreObject::Instance::AssetManager().Load<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
	IO::CoreObject::Instance::AssetManager().Load<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
	IO::CoreObject::Instance::AssetManager().Load<Asset::TextureCube>("..\\Asset\\Texture\\DefaultTextureCube.json");
	while (!_stopped)
	{
		std::this_thread::yield();
	}
}

void AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnEnd()
{
	qDebug() << "AirEngine::Core::Logic::CoreObject::Thread::LogicThread::OnEnd()";
}
