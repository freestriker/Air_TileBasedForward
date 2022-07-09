#pragma once
#include "Core/Logic/Object/Component.h"
#include <glm/glm.hpp>
#include <array>

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
			Core::Graphic::Material* material;
			void RefreshObjectInfo();
			Core::Graphic::Instance::Buffer* ObjectInfoBuffer();

			Renderer();
			virtual ~Renderer();
			Renderer(const Renderer&) = delete;
			Renderer& operator=(const Renderer&) = delete;
			Renderer(Renderer&&) = delete;
			Renderer& operator=(Renderer&&) = delete;

		private:
			ObjectInfo _objectInfo;
			Core::Graphic::Instance::Buffer* _objectInfoBuffer;

			RTTR_ENABLE(Core::Logic::Object::Component)

		};
	}
}