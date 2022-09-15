#include "Utils/IntersectionChecker.h"

void AirEngine::Utils::IntersectionChecker::SetIntersectPlanes(const glm::vec4* planes, size_t planeCount)
{
	_intersectPlanes.clear();
	_intersectPlanes.resize(planeCount);

	for (auto i = 0; i < planeCount; i++)
	{
		_intersectPlanes[i] = planes[i];
	}
}

bool AirEngine::Utils::IntersectionChecker::Check(const glm::vec3* vertexes, size_t vertexCount)
{
	int planeCount = _intersectPlanes.size();
	for (int j = 0; j < planeCount; j++)
	{
		for (int i = 0; i < vertexCount; i++)
		{
			if (glm::dot(glm::vec4(vertexes[i], 1), _intersectPlanes[j]) >= 0)
			{
				goto Out;
			}
		}
		return false;
	Out:
		continue;
	}
	return true;
}

bool AirEngine::Utils::IntersectionChecker::Check(const glm::vec3* vertexes, size_t vertexCount, glm::mat4 matrix)
{
	std::vector<glm::vec4> wvBoundryVertexes = std::vector<glm::vec4>(vertexCount);
	for (size_t i = 0; i < vertexCount; i++)
	{
		wvBoundryVertexes[i] = matrix * glm::vec4(vertexes[i], 1.0);
	}
	int planeCount = _intersectPlanes.size();
	for (int j = 0; j < planeCount; j++)
	{
		for (int i = 0; i < vertexCount; i++)
		{
			if (glm::dot(wvBoundryVertexes[i], _intersectPlanes[j]) >= 0)
			{
				goto Out;
			}
		}
		return false;
	Out:
		continue;
	}
	return true;
}

AirEngine::Utils::IntersectionChecker::IntersectionChecker()
	: _intersectPlanes()
{
}

AirEngine::Utils::IntersectionChecker::IntersectionChecker(std::vector<glm::vec4>& intersectPlanes)
	: _intersectPlanes(intersectPlanes)
{
}

AirEngine::Utils::IntersectionChecker::~IntersectionChecker()
{
}
