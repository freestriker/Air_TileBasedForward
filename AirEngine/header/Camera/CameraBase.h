#pragma once
#include "Core/Logic/Object/Component.h"
#include <glm/mat4x4.hpp>
#include <array>
#include <glm/vec4.hpp>
#include <map>
#include <string>
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
				alignas(16)	glm::vec4 parameter;
				alignas(16)	glm::vec3 forward;
				alignas(16)	glm::vec3 right;
				alignas(16)	glm::vec4 clipPlanes[6];
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
			Core::Graphic::Instance::Buffer* _buffer;
			CameraData _cameraData;
			glm::mat4 _projectionMatrix;
			Core::Graphic::Manager::RenderPassTarget* _renderPassTarget;

		public:
			glm::mat4 ViewMatrix();
			const glm::mat4* ProjectionMatrix();
			const glm::vec4* ClipPlanes();
			void RefreshCameraData();
			Core::Graphic::Instance::Buffer* CameraDataBuffer();
			void RefreshRenderPassObject();
		protected:
			CameraBase(CameraType cameraType, std::vector<std::string> renderPassNames, std::map<std::string, Core::Graphic::Instance::Image*> attachments);
			virtual ~CameraBase();
			virtual void OnSetParameter(glm::vec4& parameter) = 0;
			virtual void OnSetClipPlanes(glm::vec4* clipPlanes) = 0;
			virtual void OnSetProjectionMatrix(glm::mat4& matrix) = 0;
		private:

			RTTR_ENABLE(Core::Logic::Object::Component)
		};
	}
}
