#pragma once
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <array>

namespace AirEngine
{
	namespace Camera
	{
		class CameraBase;
	}
	namespace Rendering
	{
		namespace Utility
		{
			struct ProjectedGridInfo
			{
				glm::vec3 scale;
				glm::vec3 absDisplacement;
				glm::vec3 displacementFactor;
				float aimPointDistanceFactor;
				float aimPointHeightCompensation;
			};
			bool CalculateProjectedGridCornerPositions(const ProjectedGridInfo& projectedGridInfo, Camera::CameraBase* camera, std::array<glm::vec4, 4>& targets);
		}
	}
}