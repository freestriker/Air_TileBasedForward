#include "Rendering/Utility/ProjectedGridUtility.h"
#include <vector>
#include <glm/gtx/intersect.hpp>
#include <glm/ext/matrix_double4x4.hpp>
#include <Camera/PerspectiveCamera.h>
#include <Utils/Log.h>
#include "Core/Logic/Object/GameObject.h"

bool AirEngine::Rendering::Utility::CalculateProjectedGridCornerPositions(const ProjectedGridInfo& projectedGridInfo, Camera::CameraBase* camera, std::array<glm::vec4, 4>& targets)
{
	if (dynamic_cast<Camera::PerspectiveCamera*>(camera) == nullptr)
	{
		Utils::Log::Message("FFT ocean can only use perspective camera.");
	}
	auto& renderCamera = *dynamic_cast<Camera::PerspectiveCamera*>(camera);

	auto& cameraTransform = renderCamera.GameObject()->transform;
	const glm::dmat4&& cameraModelMatrix = cameraTransform.ModelMatrix();
	const glm::dvec3&& cameraPosition = cameraModelMatrix * glm::dvec4(0, 0, 0, 1);
	const glm::dvec3&& cameraForwardPosition = cameraModelMatrix * glm::dvec4(0, 0, -1, 1);
	const glm::dvec3&& cameraForward = glm::normalize(cameraForwardPosition - cameraPosition);
	const glm::dvec3&& cameraUpPosition = cameraModelMatrix * glm::dvec4(0, 1, 0, 1);
	const glm::dvec3&& cameraUp = glm::normalize(cameraUpPosition - cameraPosition);
	const glm::dvec3&& cameraRight = glm::normalize(glm::cross(cameraForward, cameraUp));

	const glm::dvec3&& absExtent = glm::abs(projectedGridInfo.scale * projectedGridInfo.absDisplacement);

	glm::dvec3 projectorPosition{};
	glm::dvec3 projectorForwardPosition{};
	{
		const double aimPointDistanceFactor = double(std::abs(projectedGridInfo.aimPointDistanceFactor));
		const double aimPointHeightCompensation = double(std::abs(projectedGridInfo.aimPointHeightCompensation));

		const glm::dvec3&& aimPointPosition = cameraPosition + aimPointDistanceFactor * std::abs(double(renderCamera.farFlat)) * cameraForward;

		const double cameraHeight = cameraPosition.y;
		projectorPosition = cameraPosition;
		if (cameraHeight < absExtent.y)
		{
			if (cameraHeight < 0)
			{
				projectorPosition.y += absExtent.y - 2 * cameraHeight;
			}
			else
			{
				projectorPosition.y += absExtent.y - cameraHeight;
			}
		}
		projectorPosition.y += aimPointHeightCompensation;

		projectorForwardPosition = glm::dvec3(aimPointPosition.x, 0, aimPointPosition.z);
	}

	constexpr double PI = 3.141592653589793;
	const double&& halfFov = renderCamera.fovAngle * PI / 360.0;
	const double&& cot = 1.0 / std::tan(halfFov);
	const double&& flatDistence = renderCamera.farFlat - renderCamera.nearFlat;

	const glm::dmat4&& cameraProjectionMatrix = glm::dmat4(
		cot / renderCamera.aspectRatio, 0, 0, 0,
		0, cot, 0, 0,
		0, 0, -renderCamera.farFlat / flatDistence, -1,
		0, 0, -renderCamera.nearFlat * renderCamera.farFlat / flatDistence, 0
	);

	const glm::dmat4&& cameraViewMatrix = glm::lookAt(cameraPosition, cameraForwardPosition, cameraUp);
	const glm::dmat4&& cameraInvViewMatrix = glm::inverse(cameraViewMatrix);

	const glm::dmat4&& cameraViewProjectionMatrix = cameraProjectionMatrix * cameraViewMatrix;
	const glm::dmat4&& cameraInvViewProjectionMatrix = glm::inverse(cameraViewProjectionMatrix);

	const glm::dmat4&& projectorViewMatrix = glm::lookAt(projectorPosition, projectorForwardPosition, cameraUp);
	const glm::dmat4&& projectorInvViewMatrix = glm::inverse(projectorViewMatrix);

	const glm::dmat4&& projectorViewProjectionMatrix = cameraProjectionMatrix * projectorViewMatrix;
	const glm::dmat4&& projectorInvViewProjectionMatrix = glm::inverse(projectorViewProjectionMatrix);

	std::array<glm::dvec4, 8> worldSpaceCornerPositions{};
	{
		const std::array<glm::dvec4, 8> ndcCornerPositions
		{
			glm::dvec4{-1, -1, 0, 1},
			glm::dvec4{+1, -1, 0, 1},
			glm::dvec4{-1, +1, 0, 1},
			glm::dvec4{+1, +1, 0, 1},
			glm::dvec4{-1, -1, 1, 1},
			glm::dvec4{+1, -1, 1, 1},
			glm::dvec4{-1, +1, 1, 1},
			glm::dvec4{+1, +1, 1, 1},
		};

		for (uint32_t i = 0; i < 8; ++i)
		{
			const auto& ndcCornerPosition = ndcCornerPositions.at(i);
			auto& worldSpaceCornerPosition = worldSpaceCornerPositions.at(i);

			worldSpaceCornerPosition = cameraInvViewProjectionMatrix * ndcCornerPosition;
			worldSpaceCornerPosition = worldSpaceCornerPosition / worldSpaceCornerPosition.w;
		}
	}

	const std::array<int, 24> ndcVertexIndexs = {
		0,1,	0,2,	2,3,	1,3,
		0,4,	2,6,	3,7,	1,5,
		4,6,	4,5,	5,7,	6,7
	};

	const glm::dvec3&& planeNormal{ 0, 1, 0 };
	const glm::dvec4&& upperPlane{ 0, 1, 0, -absExtent.y };
	const glm::dvec4&& lowerPlane{ 0, 1, 0, +absExtent.y };

	std::vector<glm::dvec3> projectorProjectedPositions{};
	projectorProjectedPositions.reserve(32);
	{
		for (uint32_t i = 0; i < 12; i++)
		{
			const int src = ndcVertexIndexs.at(i * 2), dst = ndcVertexIndexs.at(i * 2 + 1);
			const glm::dvec4& srcCornerPosition = worldSpaceCornerPositions.at(src);
			const glm::dvec4& dstCornerPosition = worldSpaceCornerPositions.at(dst);
			const glm::dvec3&& srcToDstDirection = glm::normalize(glm::dvec3(dstCornerPosition - srcCornerPosition));

			double distance = 0;
			if (glm::dot(upperPlane, srcCornerPosition) * glm::dot(upperPlane, dstCornerPosition) < 0)
			{
				if (glm::intersectRayPlane(glm::dvec3(srcCornerPosition), srcToDstDirection, glm::dvec3(0, -upperPlane.w, 0), planeNormal, distance))
				{
					const auto&& intersectedPosition = glm::dvec3(srcCornerPosition) + srcToDstDirection * distance;
					projectorProjectedPositions.emplace_back(intersectedPosition);
				}
			}

			if (glm::dot(lowerPlane, srcCornerPosition) * glm::dot(lowerPlane, dstCornerPosition) < 0)
			{
				if (glm::intersectRayPlane(glm::dvec3(srcCornerPosition), srcToDstDirection, glm::dvec3(0, -lowerPlane.w, 0), planeNormal, distance))
				{
					const auto&& intersectedPosition = glm::dvec3(srcCornerPosition) + srcToDstDirection * distance;
					projectorProjectedPositions.emplace_back(intersectedPosition);
				}
			}
		}

		for (int i = 0; i < 8; i++)
		{
			const glm::dvec4& cornerPosition = worldSpaceCornerPositions.at(i);
			if (glm::dot(upperPlane, cornerPosition) * glm::dot(lowerPlane, cornerPosition) < 0)
			{
				projectorProjectedPositions.emplace_back(glm::dvec3(cornerPosition));
			}
		}

		for (auto& cameraProjectedPosition : projectorProjectedPositions)
		{
			cameraProjectedPosition = cameraProjectedPosition - planeNormal * glm::dot(planeNormal, cameraProjectedPosition);
			glm::dvec4 temp = projectorViewProjectionMatrix * glm::dvec4(cameraProjectedPosition, 1);
			cameraProjectedPosition = temp / temp.w;
		}
	}

	glm::dmat4 rangeMatrix{};
	if (projectorProjectedPositions.size() == 0)
	{
		return false;
	}
	else
	{
		double x_min = projectorProjectedPositions.at(0).x;
		double x_max = projectorProjectedPositions.at(0).x;
		double y_min = projectorProjectedPositions.at(0).y;
		double y_max = projectorProjectedPositions.at(0).y;
		for (int i = 1; i < projectorProjectedPositions.size(); i++)
		{
			const auto& projectorProjectedPosition = projectorProjectedPositions.at(i);
			x_min = std::min(x_min, projectorProjectedPosition.x);
			x_max = std::max(x_max, projectorProjectedPosition.x);
			y_min = std::min(y_min, projectorProjectedPosition.y);
			y_max = std::max(y_max, projectorProjectedPosition.y);
		}

		rangeMatrix = {
			x_max - x_min	, 0				, 0, 0,
			0				, y_max - y_min	, 0, 0,
			0				, 0				, 1, 0,
			x_min			, y_min			, 0, 1
		};
	}

	const glm::dmat4&& rangeInvViewProjectionMatrix = projectorInvViewProjectionMatrix * rangeMatrix;

	{
		const std::array<glm::dvec2, 4> uvCorners
		{
			glm::dvec2{0, 0},
			glm::dvec2{1, 0},
			glm::dvec2{0, 1},
			glm::dvec2{1, 1}
		};

		std::array<glm::dvec4, 4> worldSpaceOriginalRangeHomoPositions{};
		std::array<glm::dvec3, 4> worldSpaceOriginalRangePositions{};
		for (uint32_t i = 0; i < 4; ++i)
		{
			const auto& uvCorner = uvCorners.at(i);
			auto& worldSpaceOriginalRangeHomoPosition = worldSpaceOriginalRangeHomoPositions.at(i);
			auto& worldSpaceOriginalRangePosition = worldSpaceOriginalRangePositions.at(i);

			auto&& srcPosition = rangeInvViewProjectionMatrix * glm::dvec4{ uvCorner, 0, 1 };
			auto&& dstPosition = rangeInvViewProjectionMatrix * glm::dvec4{ uvCorner, 1, 1 };
			auto&& direction = dstPosition - srcPosition;

			auto&& ratio = -srcPosition.y / direction.y;

			worldSpaceOriginalRangeHomoPosition = srcPosition + direction * ratio;

			worldSpaceOriginalRangePosition = worldSpaceOriginalRangeHomoPosition / worldSpaceOriginalRangeHomoPosition.w;
		}

		const double DisplacementX = projectedGridInfo.scale.x * projectedGridInfo.absDisplacement.x * projectedGridInfo.displacementFactor.x;
		const double DisplacementZ = projectedGridInfo.scale.z * projectedGridInfo.absDisplacement.z * projectedGridInfo.displacementFactor.z;

		std::array<glm::dvec3, 4> worldSpaceDisplacedRangePositions{};
		worldSpaceDisplacedRangePositions.at(0) = worldSpaceOriginalRangePositions.at(0) - DisplacementX * cameraRight - DisplacementZ * cameraForward;
		worldSpaceDisplacedRangePositions.at(1) = worldSpaceOriginalRangePositions.at(1) + DisplacementX * cameraRight - DisplacementZ * cameraForward;
		worldSpaceDisplacedRangePositions.at(2) = worldSpaceOriginalRangePositions.at(2) - DisplacementX * cameraRight + DisplacementZ * cameraForward;
		worldSpaceDisplacedRangePositions.at(3) = worldSpaceOriginalRangePositions.at(3) + DisplacementX * cameraRight + DisplacementZ * cameraForward;


		std::array<glm::dvec4, 4> worldSpaceDisplacedRangeHomoPositions{};
		for (uint32_t i = 0; i < 4; ++i)
		{
			auto& targetRangePosition = worldSpaceDisplacedRangeHomoPositions.at(i);
			const auto& worldSpaceDisplacedRangePosition = worldSpaceDisplacedRangePositions.at(i);

			targetRangePosition = projectorViewProjectionMatrix * glm::dvec4(worldSpaceDisplacedRangePosition, 1);
			targetRangePosition = targetRangePosition / targetRangePosition.w;
			targetRangePosition = projectorInvViewProjectionMatrix * targetRangePosition;
		}

		for (uint32_t i = 0; i < 4; ++i)
		{
			targets.at(i) = worldSpaceDisplacedRangeHomoPositions.at(i);
		}
	}

	return true;
}
