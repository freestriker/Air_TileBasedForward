#pragma once
#include <random>
#include <glm/vec3.hpp>

namespace AirEngine
{
	namespace Utils
	{
		class RandomSphericalCoordinateGenerator
		{
		public:
			RandomSphericalCoordinateGenerator(float minTheta, float maxTheta, float minPhi, float maxPhi, float radius);
			RandomSphericalCoordinateGenerator();
			~RandomSphericalCoordinateGenerator();
			RandomSphericalCoordinateGenerator(const RandomSphericalCoordinateGenerator&) = delete;
			RandomSphericalCoordinateGenerator& operator=(const RandomSphericalCoordinateGenerator&) = delete;
			RandomSphericalCoordinateGenerator(RandomSphericalCoordinateGenerator&&) = delete;
			RandomSphericalCoordinateGenerator& operator=(RandomSphericalCoordinateGenerator&&) = delete;
		private:
			std::default_random_engine _engine;
			std::uniform_real_distribution<float> _thetaDistribution;
			std::uniform_real_distribution<float> _phiDistribution;

		public:
			float radius;
			static const float pi;
		public:
			glm::vec3 Get();
		};
	}
}

