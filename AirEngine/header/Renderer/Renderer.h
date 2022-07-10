#pragma once
#include "Core/Logic/Object/Component.h"
#include <glm/glm.hpp>
#include <array>
#include <map>
#include <string>
namespace AirEngine
{
	namespace Asset
	{
		class Mesh;
	}
	namespace Core
	{
		namespace Graphic
		{
			class Material;
			namespace Instance
			{
				class Buffer;
			}
		}
	}
	namespace Renderer
	{
		class Renderer : public Core::Logic::Object::Component
		{
		public:
			struct ObjectInfo
			{
				alignas(16) glm::mat4 model;
				alignas(16) glm::mat4 itModel;
			};
			bool enableFrustumCulling;
			Asset::Mesh* mesh;
			void RefreshObjectInfo();
			Core::Graphic::Instance::Buffer* ObjectInfoBuffer();

			void AddMaterial(Core::Graphic::Material* material);
			Core::Graphic::Material* GetMaterial(std::string pass);
			const std::map<std::string, Core::Graphic::Material*>* GetMAterials();

			Renderer();
			virtual ~Renderer();
			Renderer(const Renderer&) = delete;
			Renderer& operator=(const Renderer&) = delete;
			Renderer(Renderer&&) = delete;
			Renderer& operator=(Renderer&&) = delete;

		private:
			ObjectInfo _objectInfo;
			Core::Graphic::Instance::Buffer* _objectInfoBuffer;
			std::map<std::string, Core::Graphic::Material*> _materials;
			RTTR_ENABLE(Core::Logic::Object::Component)

		};
	}
}