#pragma once
#include "Core/IO/Asset/AssetBase.h"
#include <glm/glm.hpp>
#include <vector>
namespace AirEngine
{
	namespace Utils
	{
		class OrientedBoundingBox;
	}
	namespace Core
	{
		namespace Graphic
		{
			namespace Instance
			{
				class Buffer;
			}
			namespace Command
			{
				class CommandBuffer;
			}
		}
	}
	namespace Asset
	{
		struct VertexData
		{
			glm::vec3 position;
			glm::vec2 texCoords;
			glm::vec3 normal;
			glm::vec3 tangent;
			glm::vec3 bitangent;
		};

		class Mesh final : public Core::IO::Asset::AssetBsse
		{
		private:
			std::vector<VertexData> _vertices;
			std::vector<uint32_t> _indices;
			Core::Graphic::Instance::Buffer* _vertexBuffer;
			Core::Graphic::Instance::Buffer* _indexBuffer;
			Utils::OrientedBoundingBox* _orientedBoundingBox;
			void OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)override;
		public:
			Mesh();
			Mesh(Core::Graphic::Command::CommandBuffer* transferCommandBuffer, std::vector<VertexData> vertices, std::vector<uint32_t> indices);
			~Mesh();
			Core::Graphic::Instance::Buffer& VertexBuffer();
			Core::Graphic::Instance::Buffer& IndexBuffer();
			std::vector<VertexData>& Vertices();
			std::vector<uint32_t>& Indices();
			Utils::OrientedBoundingBox& OrientedBoundingBox();
		};
	}
}