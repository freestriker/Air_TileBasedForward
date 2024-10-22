#pragma once
#include "Camera/CameraBase.h"

namespace AirEngine
{
	namespace Camera
	{
		class OrthographicCamera final : public Camera::CameraBase
		{
		private:
			void OnSetSize(glm::vec2& parameter)override;
			void OnSetClipPlanes(glm::vec4* clipPlanes)override;
			void OnSetProjectionMatrix(glm::mat4& matrix)override;

			OrthographicCamera(const OrthographicCamera&) = delete;
			OrthographicCamera& operator=(const OrthographicCamera&) = delete;
			OrthographicCamera(OrthographicCamera&&) = delete;
			OrthographicCamera& operator=(OrthographicCamera&&) = delete;
		public:
			float size;
			OrthographicCamera(std::string rendererName, std::map<std::string, Core::Graphic::Instance::Image*> attachments);
			virtual ~OrthographicCamera();

			RTTR_ENABLE(Camera::CameraBase)
		};
	}
}