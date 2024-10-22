#pragma once
#include <glm/glm.hpp>
#include <vector>

namespace AirEngine
{
	namespace Utils
	{
		class IntersectionChecker final
		{
		private:
			std::vector<glm::vec4> _intersectPlanes;
		public:
			void SetIntersectPlanes(const glm::vec4* planes, size_t planeCount);
			bool Check(const glm::vec3* vertexes, size_t vertexCount);
			bool Check(const glm::vec3* vertexes, size_t vertexCount, glm::mat4 matrix);

			IntersectionChecker();
			IntersectionChecker(std::vector<glm::vec4>& intersectPlanes);
			~IntersectionChecker();
		};
	}
}
