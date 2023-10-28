#pragma once
#include "Core/Graphic/Rendering/RenderFeatureBase.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/ext/vector_int2.hpp>
#include <qobject.h>
#include <qwidget.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include "Utils/Log.h"
#include <QFormLayout>
#include "Utils/DataWidgetLauncher.h"
#include "Core/Graphic/Rendering/RenderPassBase.h"

namespace AirEngine
{
	namespace Asset
	{
		class Mesh;
	}
	namespace Core
	{
		namespace Graphic
		{
			namespace Rendering
			{
				class Shader;
				class Material;
			}
			namespace Instance
			{
				class ImageSampler;
			}
		}
	}
	namespace Rendering
	{
		namespace RenderFeature
		{
			class GerstnerOcean_RenderFeature final : public Core::Graphic::Rendering::RenderFeatureBase
			{
			public:
				class GerstnerOcean_Surface_RenderPass final : public Core::Graphic::Rendering::RenderPassBase
				{
				private:
					void OnPopulateRenderPassSettings(RenderPassSettings& settings)override;
				public:
					CONSTRUCTOR(GerstnerOcean_Surface_RenderPass)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderPassBase)
				};
				class GerstnerOceanDataWidgetLauncher;
				class GerstnerOcean_RenderFeatureData final : public Core::Graphic::Rendering::RenderFeatureDataBase
				{
					friend class GerstnerOcean_RenderFeature;
				public:
					struct SubGerstnerWaveInfo
					{
						float amplitudeFactor;
						float windRotationAngle;
						float waveLength;
						float omegaFactor;
						float phiAngle;
					};
					bool isDirty;
					std::vector< SubGerstnerWaveInfo> subGerstnerWaveInfos;
					glm::vec3 displacementFactor;
					glm::vec3 oceanScale;
					glm::vec3 absDisplacement;
					float aimPointDistanceFactor;
					float aimPointHeightCompensation;
					bool showWireFrame;

					GerstnerOceanDataWidgetLauncher* widgetLauncher;

					Core::Graphic::Rendering::FrameBuffer* frameBuffer;

					Core::Graphic::Instance::Buffer* gerstnerWaveInfoStagingBuffer;
					Core::Graphic::Instance::Buffer* gerstnerWaveInfoBuffer;

					Asset::Mesh* surfaceMesh;
					Core::Graphic::Rendering::Shader* surfaceShader;
					Core::Graphic::Rendering::Material* surfaceMaterial;
					Core::Graphic::Rendering::Shader* surfaceWireFrameShader;
					Core::Graphic::Rendering::Material* surfaceWireFrameMaterial;
				public:

					CONSTRUCTOR(GerstnerOcean_RenderFeatureData)
					RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureDataBase)
				};
				class GerstnerOceanDataWidgetLauncher final : public AirEngine::Utils::DataWidgetLauncher< GerstnerOcean_RenderFeatureData>
				{
				private:
					void OnSetUp() override;
				public:
					GerstnerOceanDataWidgetLauncher(GerstnerOcean_RenderFeatureData& data);
				};

				CONSTRUCTOR(GerstnerOcean_RenderFeature)

			private:
				Core::Graphic::Rendering::RenderPassBase* _renderPass;
				std::string _renderPassName;

				Core::Graphic::Rendering::RenderFeatureDataBase* OnCreateRenderFeatureData(Camera::CameraBase* camera)override;
				void OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)override;
				void OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)override;

				void OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)override;
				void OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*>const* rendererComponents)override;
				void OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)override;
				void OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)override;

				RTTR_ENABLE(Core::Graphic::Rendering::RenderFeatureBase)
			};
		}
	}
}