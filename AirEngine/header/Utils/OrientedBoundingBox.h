#pragma once
#include <glm/mat3x3.hpp>
#include <glm/vec3.hpp>
#include <vector>
#include <algorithm>
#include <array>
namespace AirEngine
{
    namespace Utils
    {
        class OrientedBoundingBox
        {
        private:
            static glm::mat3 ComputeCovarianceMatrix(std::vector<glm::vec3>& positions);
            static void JacobiSolver(glm::mat3& matrix, float* eValue, glm::vec3* eVectors);
            static void SchmidtOrthogonal(glm::vec3& v0, glm::vec3& v1, glm::vec3& v2);
            glm::vec3 _center;
            std::array<glm::vec3, 3> _directions;
            glm::vec3 _halfEdgeLength;
            std::array<glm::vec3, 8> _boundryVertexes;
        public:
            const glm::vec3& Center();
            const std::array<glm::vec3, 3>& Directions();
            const glm::vec3& HalfEdgeLength();
            const std::array<glm::vec3, 8>& BoundryVertexes();

            OrientedBoundingBox(std::vector<glm::vec3>& positions);
        };
    }
}
