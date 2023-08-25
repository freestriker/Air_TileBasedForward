#include "Test/IfftOcean_WindRotationBehaviour.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Renderer/Renderer.h"
#include "Utils/Time.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Logic/Manager/InputManager.h"
#include "Core/Logic/CoreObject/Instance.h"
#include "Rendering/RenderFeature/FftOcean_RenderFeature.h"
#include "Camera/CameraBase.h"
#include "Core/Graphic/Rendering/RendererBase.h"
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Test::IfftOcean_WindRotationBehaviour>("AirEngine::Test::IfftOcean_WindRotationBehaviour");
}

AirEngine::Test::IfftOcean_WindRotationBehaviour::IfftOcean_WindRotationBehaviour()
	: windRotationAngleSpeed(30)
{
}

AirEngine::Test::IfftOcean_WindRotationBehaviour::~IfftOcean_WindRotationBehaviour()
{
}

void AirEngine::Test::IfftOcean_WindRotationBehaviour::OnAwake()
{
}

void AirEngine::Test::IfftOcean_WindRotationBehaviour::OnStart()
{
}

void AirEngine::Test::IfftOcean_WindRotationBehaviour::OnUpdate()
{
	auto& featureData = *static_cast<AirEngine::Rendering::RenderFeature::FftOcean_RenderFeature::FftOcean_RenderFeatureData*>(Camera::CameraBase::mainCamera->RendererData()->RenderFeatureData("FftOcean_RenderFeature"));
	if (Core::Logic::CoreObject::Instance::InputManager().KeyStatus(Core::Logic::Manager::InputKeyType::Key_P) == Core::Logic::Manager::ButtonStatusType::Pressed)
	{
		featureData.windRotationAngle += Core::Logic::CoreObject::Instance::time.DeltaDuration() * windRotationAngleSpeed;
		featureData.windRotationAngle = std::fmod(featureData.windRotationAngle, 360);
		Utils::Log::Message("windRotationAngle: " + ::std::to_string(featureData.windRotationAngle));
	}
}

void AirEngine::Test::IfftOcean_WindRotationBehaviour::OnDestroy()
{
}
