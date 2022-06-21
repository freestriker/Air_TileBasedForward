#pragma once
#include "Core/Logic/Object/Component.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Utils/ChildBrotherTree.h"

namespace AirEngine
{
	namespace Core
	{
		namespace Logic
		{
			namespace Object
			{
				class GameObject;
				class Transform final: public Component, public Utils::ChildBrotherTree<Transform>
				{
				private:
					bool Active();
					void SetActive(bool);

					void UpdateModelMatrix(glm::mat4& parentModelMatrix);
					glm::vec3 _rotation;
					glm::vec3 _translation;
					glm::vec3 _scale;

					glm::mat4 _modelMatrix;
					glm::mat4 _relativeModelMatrix;

				public:
					void SetTranslation(glm::vec3 translation);
					void SetRotation(glm::vec3 rotation);
					void SetEulerRotation(glm::vec3 rotation);
					void SetScale(glm::vec3 scale);
					void SetTranslationRotationScale(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale);

					glm::mat4 TranslationMatrix();
					glm::mat4 RotationMatrix();
					glm::mat4 ScaleMatrix();
					glm::mat4 ModelMatrix();

					glm::vec3 Rotation();
					glm::vec3 EulerRotation();
					glm::vec3 Translation();
					glm::vec3 Scale();

					Transform();
					virtual ~Transform();

					RTTR_ENABLE(Component)

				};
			}
		}
	}
}

