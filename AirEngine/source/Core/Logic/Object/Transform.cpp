#include "Core/Logic/Object/Transform.h"
#include "Core/Logic/Object/GameObject.h"
#include <rttr/registration>
#include <glm/glm.hpp>
RTTR_REGISTRATION
{
    using namespace rttr;
    registration::class_<AirEngine::Core::Logic::Object::Transform>("AirEngine::Core::Logic::Object::Transform");
}

AirEngine::Core::Logic::Object::Transform::Transform()
    : Component(Component::ComponentType::TRANSFORM)
    , _translation(glm::vec3(0, 0, 0))
    , _rotation(glm::vec3(0, 0, 0))
    , _scale(glm::vec3(1, 1, 1))
    , _relativeModelMatrix(glm::mat4(1))
    , _modelMatrix(glm::mat4(1))
{

}

AirEngine::Core::Logic::Object::Transform::~Transform()
{
}

bool AirEngine::Core::Logic::Object::Transform::Active()
{
    return true;
}

void AirEngine::Core::Logic::Object::Transform::SetActive(bool)
{
}

void AirEngine::Core::Logic::Object::Transform::UpdateModelMatrix(glm::mat4& parentModelMatrix)
{
    _modelMatrix = parentModelMatrix * _relativeModelMatrix;
    auto child = Child();
    while (child)
    {
        child->UpdateModelMatrix(_modelMatrix);

        child = child->Brother();
    }
}

void AirEngine::Core::Logic::Object::Transform::SetTranslation(glm::vec3 translation)
{
    this->_translation = translation;

    _relativeModelMatrix = TranslationMatrix() * RotationMatrix() * ScaleMatrix();
    UpdateModelMatrix(Parent()->_modelMatrix);
}

void AirEngine::Core::Logic::Object::Transform::SetRotation(glm::vec3 rotation)
{
    this->_rotation = rotation;

    _relativeModelMatrix = TranslationMatrix() * RotationMatrix() * ScaleMatrix();
    UpdateModelMatrix(Parent()->_modelMatrix);
}

void AirEngine::Core::Logic::Object::Transform::SetEulerRotation(glm::vec3 rotation)
{
    double k = std::acos(-1.0) / 180.0;
    this->_rotation = rotation * static_cast<float>(k);

    _relativeModelMatrix = TranslationMatrix() * RotationMatrix() * ScaleMatrix();
    UpdateModelMatrix(Parent()->_modelMatrix);
}

void AirEngine::Core::Logic::Object::Transform::SetScale(glm::vec3 scale)
{
    this->_scale = scale;

    _relativeModelMatrix = TranslationMatrix() * RotationMatrix() * ScaleMatrix();
    UpdateModelMatrix(Parent()->_modelMatrix);
}

void AirEngine::Core::Logic::Object::Transform::SetTranslationRotationScale(glm::vec3 translation, glm::vec3 rotation, glm::vec3 scale)
{
    this->_translation = translation;
    this->_rotation = rotation;
    this->_scale = scale;

    _relativeModelMatrix = TranslationMatrix() * RotationMatrix() * ScaleMatrix();
    UpdateModelMatrix(Parent()->_modelMatrix);
}

glm::mat4 AirEngine::Core::Logic::Object::Transform::TranslationMatrix()
{
    return glm::mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        _translation.x, _translation.y, _translation.z, 1
    );
}

glm::mat4 AirEngine::Core::Logic::Object::Transform::RotationMatrix()
{
    return glm::rotate(glm::rotate(glm::rotate(glm::mat4(1), _rotation.x, { 1, 0, 0 }), _rotation.y, { 0, 1, 0 }), _rotation.z, { 0, 0, 1 });
}

glm::mat4 AirEngine::Core::Logic::Object::Transform::ScaleMatrix()
{
    return glm::mat4(
        _scale.x, 0, 0, 0,
        0, _scale.y, 0, 0,
        0, 0, _scale.z, 0,
        0, 0, 0, 1
    );
}

glm::mat4 AirEngine::Core::Logic::Object::Transform::ModelMatrix()
{
    return _modelMatrix;
}

glm::vec3 AirEngine::Core::Logic::Object::Transform::Rotation()
{
    return _rotation;
}

glm::vec3 AirEngine::Core::Logic::Object::Transform::EulerRotation()
{
    const double k = 180.0 / std::acos(-1.0);
    return _rotation * static_cast<float>(k);
}

glm::vec3 AirEngine::Core::Logic::Object::Transform::Translation()
{
    return _translation;
}

glm::vec3 AirEngine::Core::Logic::Object::Transform::Scale()
{
    return _scale;
}
