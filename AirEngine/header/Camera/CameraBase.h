#pragma once
#include "Core/Logic/Object/Component.h"
#include <glm/mat4x4.hpp>
#include <array>
#include <glm/vec4.hpp>
#include <map>
#include <string>
#include "Utils/IntersectionChecker.h"
#include <array>

namespace AirEngine
{
	namespace Asset
	{
		class Mesh;
	}
	namespace Core::Graphic
	{
		namespace Instance
		{
			class Buffer;
			class Image;
			class ImageSampler;
		}
		namespace Command
		{
			class CommandBuffer;
		}
		namespace CoreObject
		{
			class Thread;
		}
		namespace Manager
		{
			class RenderPassTarget;
		}
	}
	namespace Camera
	{
		class CameraBase : public Core::Logic::Object::Component
		{
			friend class Core::Graphic::CoreObject::Thread;
		public:
			enum class CameraType
			{
				ORTHOGRAPHIC = 1,
				PERSPECTIVE = 2
			};
			struct CameraData
			{
				alignas(4)	int type;
				alignas(4)	float nearFlat;
				alignas(4)	float farFlat;
				alignas(4)	float aspectRatio;
				alignas(16)	glm::vec3 position;
				alignas(8)	glm::vec2 halfSize;
				alignas(16)	glm::vec4 parameter;
				alignas(16)	glm::vec3 forward;
				alignas(16)	glm::vec3 right;
				alignas(16)	glm::vec4 clipPlanes[6];
				alignas(16) glm::mat4 projection;
				alignas(16) glm::mat4 view;
			};
		public:
			static CameraBase* mainCamera;
			const CameraType cameraType;
			std::vector<std::string> renderPassNames;
			std::map<std::string, Core::Graphic::Instance::Image*> attachments;
			float nearFlat;
			float farFlat;
			float aspectRatio;
		protected:
		private:
			std::string _rendererName;
			Core::Graphic::Instance::Buffer* _buffer;
			CameraData _cameraInfo;
			glm::mat4 _projectionMatrix;
			Core::Graphic::Manager::RenderPassTarget* _renderPassTarget;
			AirRenderer::Utils::IntersectionChecker _intersectionChecker;

		public:
			glm::mat4 ViewMatrix();
			glm::mat4 ProjectionMatrix();
			const glm::vec4* ClipPlanes();
			void RefreshCameraInfo();
			Core::Graphic::Instance::Buffer* CameraInfoBuffer();
			void RefreshRenderPassObject();
			bool CheckInFrustum(std::array<glm::vec3, 8>& vertexes, glm::mat4& matrix);
			Core::Graphic::Manager::RenderPassTarget* RenderPassTarget();

			void SetRendererName(std::string rendererName);
			std::string RendererName();
			void RefreshRenderer();
		protected:
			CameraBase(CameraType cameraType, std::vector<std::string> renderPassNames, std::map<std::string, Core::Graphic::Instance::Image*> attachments);
			virtual ~CameraBase();
			virtual void OnSetParameter(glm::vec4& parameter);
			virtual void OnSetSize(glm::vec2& parameter) = 0;
			virtual void OnSetClipPlanes(glm::vec4* clipPlanes) = 0;
			virtual void OnSetProjectionMatrix(glm::mat4& matrix) = 0;
		private:

			RTTR_ENABLE(Core::Logic::Object::Component)
		};
	}
}
