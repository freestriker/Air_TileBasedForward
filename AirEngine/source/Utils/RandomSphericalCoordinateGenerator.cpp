#include "Utils/RandomSphericalCoordinateGenerator.h"
#include <algorithm>

const float AirEngine::Utils::RandomSphericalCoordinateGenerator::pi = std::acos(-1.0);

AirEngine::Utils::RandomSphericalCoordinateGenerator::RandomSphericalCoordinateGenerator(float minTheta, float maxTheta, float minPhi, float maxPhi, float radius)
	: _engine()
	, _thetaDistribution(std::clamp(std::cos(maxTheta / 180.0f * pi), 0.0f, 1.0f), std::clamp(std::cos(minTheta / 180.0f * pi), 0.0f, 1.0f))
	, _phiDistribution(std::clamp(minPhi / 360.0f, 0.0f, 1.0f), std::clamp(maxPhi / 360.0f, 0.0f, 1.0f))
	, radius(radius)
{
}

AirEngine::Utils::RandomSphericalCoordinateGenerator::RandomSphericalCoordinateGenerator()
	: RandomSphericalCoordinateGenerator(0, 180, 0, 360, 1)
{
}

AirEngine::Utils::RandomSphericalCoordinateGenerator::~RandomSphericalCoordinateGenerator()
{
}

glm::vec3 AirEngine::Utils::RandomSphericalCoordinateGenerator::Get()
{
	float t = std::acos(1.0f - 2 * _thetaDistribution(_engine));
	float p = 2 * pi * _phiDistribution(_engine);
	return glm::vec3(std::cos(p), std::sin(p), std::cos(t)) * radius;
}
