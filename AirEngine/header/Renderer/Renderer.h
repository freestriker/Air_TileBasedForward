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
			struct MatrixData
			{
				alignas(16) glm::mat4 model;
				alignas(16) glm::mat4 view;
				alignas(16) glm::mat4 projection;
				alignas(16) glm::mat4 itModel;
			};
			Core::Graphic::Instance::Buffer* _matrixBuffer;
			bool enableFrustumCulling;
			Asset::Mesh* mesh;
			Core::Graphic::Material* material;
			void SetMatrixData(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
			Renderer();
			virtual ~Renderer();
			Renderer(const Renderer&) = delete;
			Renderer& operator=(const Renderer&) = delete;
			Renderer(Renderer&&) = delete;
			Renderer& operator=(Renderer&&) = delete;

			RTTR_ENABLE(Core::Logic::Object::Component)

		};
	}
}