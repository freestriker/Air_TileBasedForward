#pragma once
#include "Camera/CameraBase.h"

namespace AirEngine
{
	namespace Camera
	{
		class PerspectiveCamera final : public Camera::CameraBase
		{
		private:
			void OnSetSize(glm::vec2& parameter)override;
			void OnSetClipPlanes(glm::vec4* clipPlanes)override;
			void OnSetProjectionMatrix(glm::mat4& matrix)override;

			PerspectiveCamera(const PerspectiveCamera&) = delete;
			PerspectiveCamera& operator=(const PerspectiveCamera&) = delete;
			PerspectiveCamera(PerspectiveCamera&&) = delete;
			PerspectiveCamera& operator=(PerspectiveCamera&&) = delete;
		public:
			float fovAngle;
			PerspectiveCamera(std::string rendererName, std::map<std::string, Core::Graphic::Instance::Image*> attachments);
			virtual ~PerspectiveCamera();

			RTTR_ENABLE(Camera::CameraBase)
		};
	}
}