#include "Rendering/RenderFeature/GerstnerOcean_RenderFeature.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Core/Graphic/Manager/RenderPassManager.h"
#include "Core/Graphic/Rendering/FrameBuffer.h"
#include "Camera/CameraBase.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include "Asset/Mesh.h"
#include "Renderer/Renderer.h"
#include "Utils/OrientedBoundingBox.h"
#include "Core/Logic/Object/GameObject.h"
#include "Core/Logic/Object/Transform.h"
#include "Core/Graphic/Rendering/Material.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Core/Graphic/Manager/LightManager.h"
#include "Core/IO/CoreObject/Instance.h"
#include "Core/IO/Manager/AssetManager.h"
#include "Core/Graphic/Command/BufferMemoryBarrier.h"
#include "Rendering/RenderFeature/CSM_ShadowCaster_RenderFeature.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/Instance/Memory.h"
#include <cmath>
#include <corecrt_math.h>
#include <Core/Logic/CoreObject/Instance.h>
#include "Core/Logic/Manager/InputManager.h"
#include "Core/Graphic/CoreObject/Window.h"
#include <random>
#include <qlineedit.h>
#include <QIntValidator>
#include <QDoubleValidator>
#include <glm/gtx/intersect.hpp>
#include <glm/ext/matrix_double4x4.hpp>
#include <Camera/PerspectiveCamera.h>
#include <QCheckBox>
#include <Rendering/Utility/ProjectedGridUtility.h>

RTTR_REGISTRATION
{
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_Surface_RenderPass>("AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_Surface_RenderPass")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_RenderFeatureData>("AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_RenderFeatureData")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
	rttr::registration::class_<AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature>("AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;
}

AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_Surface_RenderPass::GerstnerOcean_Surface_RenderPass()
	: RenderPassBase()
{
}

AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_Surface_RenderPass::~GerstnerOcean_Surface_RenderPass()
{

}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_Surface_RenderPass::OnPopulateRenderPassSettings(RenderPassSettings& settings)
{
	settings.AddColorAttachment(
		"ColorAttachment",
		VK_FORMAT_R8G8B8A8_SRGB,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	);
	settings.AddDepthAttachment(
		"DepthAttachment",
		VK_FORMAT_D32_SFLOAT,
		VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT,
		VK_ATTACHMENT_LOAD_OP_LOAD,
		VK_ATTACHMENT_STORE_OP_STORE,
		VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
	);
	settings.AddSubpass(
		"DrawSubpass",
		VK_PIPELINE_BIND_POINT_GRAPHICS,
		{ "ColorAttachment" },
		"DepthAttachment"
	);
	//settings.AddDependency(
	//	"VK_SUBPASS_EXTERNAL",
	//	"DrawSubpass",
	//	VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	//	VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	//	VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	//	VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	//);
	//settings.AddDependency(
	//	"DrawSubpass",
	//	"VK_SUBPASS_EXTERNAL",
	//	VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	//	VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
	//	VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
	//	VkAccessFlagBits::VK_ACCESS_COLOR_ATTACHMENT_READ_BIT
	//);
}

AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_RenderFeatureData::GerstnerOcean_RenderFeatureData()
	: RenderFeatureDataBase()
{

}

AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_RenderFeatureData::~GerstnerOcean_RenderFeatureData()
{

}
AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOcean_RenderFeature()
	: RenderFeatureBase()
	, _renderPass(Core::Graphic::CoreObject::Instance::RenderPassManager().LoadRenderPass<GerstnerOcean_Surface_RenderPass>())
	, _renderPassName(rttr::type::get<GerstnerOcean_Surface_RenderPass>().get_name().to_string())
{
	_description = "Use Gerstner and Projected-Grid to render ocean.";
}

AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::~GerstnerOcean_RenderFeature()
{
	Core::Graphic::CoreObject::Instance::RenderPassManager().UnloadRenderPass<GerstnerOcean_Surface_RenderPass>();
}

AirEngine::Core::Graphic::Rendering::RenderFeatureDataBase* AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnCreateRenderFeatureData(Camera::CameraBase* camera)
{
	GerstnerOcean_RenderFeatureData* featureDataPtr = new GerstnerOcean_RenderFeatureData();
	GerstnerOcean_RenderFeatureData& featureData = *featureDataPtr;

	featureData.isDirty = true;

	featureData.subGerstnerWaveInfos.reserve(5);
	featureData.subGerstnerWaveInfos.resize(1);
	featureData.subGerstnerWaveInfos.at(0).amplitudeFactor = 1;
	featureData.subGerstnerWaveInfos.at(0).windRotationAngle = 0;
	featureData.subGerstnerWaveInfos.at(0).waveLength = 1;
	featureData.subGerstnerWaveInfos.at(0).omegaFactor = 1;
	featureData.subGerstnerWaveInfos.at(0).phiAngle = 0;

	featureData.displacementFactor = { 1, 1, 1 };
	featureData.oceanScale = { 100, 100, 100 };
	featureData.absDisplacement = glm::vec3(0.12, 0.12, 0.12);
	featureData.aimPointDistanceFactor = 10;
	featureData.aimPointHeightCompensation = 20;
	featureData.showWireFrame = false;

	featureData.widgetLauncher = new GerstnerOceanDataWidgetLauncher(featureData);
	featureData.widgetLauncher->Launch();

	featureData.frameBuffer = new Core::Graphic::Rendering::FrameBuffer(_renderPass, camera->attachments);

	featureData.gerstnerWaveInfoStagingBuffer = nullptr;

	featureData.gerstnerWaveInfoBuffer = nullptr;

	//featureData.gerstnerWaveInfoStagingBuffer = new Core::Graphic::Instance::Buffer(
	//	sizeof(GerstnerOcean_RenderFeatureData::SubGerstnerWaveInfo) * featureData.subGerstnerWaveInfos.size(),
	//	VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
	//	VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	//);

	//featureData.gerstnerWaveInfoBuffer = new Core::Graphic::Instance::Buffer(
	//	sizeof(GerstnerOcean_RenderFeatureData::SubGerstnerWaveInfo) * featureData.subGerstnerWaveInfos.size(),
	//	VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
	//	VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	//);

	//featureData.surfaceMesh = Core::IO::CoreObject::Instance::AssetManager().Load<Asset::Mesh>("..\\Asset\\Mesh\\Surface.ply");
	//featureData.surfaceShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\GerstnerOcean_Surface_Shader.shader");
	//featureData.surfaceMaterial = new Core::Graphic::Rendering::Material(featureData.surfaceShader);
	//featureData.surfaceMaterial->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
	//featureData.surfaceMaterial->SetStorageBuffer("gerstnerWaveInfoBuffer", featureData.gerstnerWaveInfoBuffer);

	//featureData.surfaceWireFrameShader = Core::IO::CoreObject::Instance::AssetManager().Load<Core::Graphic::Rendering::Shader>("..\\Asset\\Shader\\GerstnerOcean_SurfaceWireFrame_Shader.shader");
	//featureData.surfaceWireFrameMaterial = new Core::Graphic::Rendering::Material(featureData.surfaceWireFrameShader);
	//featureData.surfaceWireFrameMaterial->SetUniformBuffer("cameraInfo", camera->CameraInfoBuffer());
	//featureData.surfaceMaterial->SetStorageBuffer("gerstnerWaveInfoBuffer", featureData.gerstnerWaveInfoBuffer);

	return featureDataPtr;
}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnResolveRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Camera::CameraBase* camera)
{
}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnDestroyRenderFeatureData(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
	auto featureData = static_cast<GerstnerOcean_RenderFeatureData*>(renderFeatureData);

	featureData->widgetLauncher->Terminate();

	delete featureData->frameBuffer;

	//delete featureData->surfaceMaterial;
	//Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\GerstnerOcean_Surface_Shader.shader");
	//delete featureData->surfaceWireFrameMaterial;
	//Core::IO::CoreObject::Instance::AssetManager().Unload("..\\Asset\\Shader\\GerstnerOcean_SurfaceWireFrame_Shader.shader");

	//delete featureData->gerstnerWaveInfoStagingBuffer;
	//delete featureData->gerstnerWaveInfoBuffer;

	delete featureData;
}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnPrepare(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData)
{
}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnExcute(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer, Camera::CameraBase* camera, std::vector<AirEngine::Renderer::Renderer*> const* rendererComponents)
{
}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnSubmit(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::OnFinish(Core::Graphic::Rendering::RenderFeatureDataBase* renderFeatureData, Core::Graphic::Command::CommandBuffer* commandBuffer)
{
}

AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOceanDataWidgetLauncher::GerstnerOceanDataWidgetLauncher(GerstnerOcean_RenderFeatureData& data)
	: AirEngine::Utils::DataWidgetLauncher< GerstnerOcean_RenderFeatureData>(data)
{

}

void AirEngine::Rendering::RenderFeature::GerstnerOcean_RenderFeature::GerstnerOceanDataWidgetLauncher::OnSetUp()
{
	auto&& widget = Widget();
	auto&& gerstnerOceanDataPtr = &Data();

	widget->setWindowTitle(QStringLiteral("GerstnerOceanDataWidget"));

	QIntValidator* intValidator = new QIntValidator;
	intValidator->setRange(0, 10000000);

	QDoubleValidator* doubleValidator = new QDoubleValidator;
	doubleValidator->setRange(0, 10000000, 5);

	QFormLayout* pLayout = new QFormLayout(widget);

	// Scale
	{
		pLayout->addRow(QStringLiteral("-----Scale :-----"), new QWidget(widget));

		// displacementFactor
		{
			QGridLayout* gridLayout = new QGridLayout();
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(gerstnerOceanDataPtr->displacementFactor.x)), widget);
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, widget, [gerstnerOceanDataPtr](const QString& string)->void {
					gerstnerOceanDataPtr->displacementFactor.x = string.toFloat();
					Utils::Log::Message("displacementFactor.x: " + std::to_string(gerstnerOceanDataPtr->displacementFactor.x));
					});
				gridLayout->addWidget(lineEdit, 0, 0);
			}
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(gerstnerOceanDataPtr->displacementFactor.y)), widget);
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, widget, [gerstnerOceanDataPtr](const QString& string)->void {
					gerstnerOceanDataPtr->displacementFactor.y = string.toFloat();
					Utils::Log::Message("displacementFactor.y: " + std::to_string(gerstnerOceanDataPtr->displacementFactor.y));
					});
				gridLayout->addWidget(lineEdit, 0, 1);
			}
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(gerstnerOceanDataPtr->displacementFactor.z)), widget);
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, widget, [gerstnerOceanDataPtr](const QString& string)->void {
					gerstnerOceanDataPtr->displacementFactor.z = string.toFloat();
					Utils::Log::Message("displacementFactor.z: " + std::to_string(gerstnerOceanDataPtr->displacementFactor.z));
					});
				gridLayout->addWidget(lineEdit, 0, 2);
			}
			pLayout->addRow(QStringLiteral("displacementFactor: "), gridLayout);
		}

		// absDisplacement
		{
			QGridLayout* gridLayout = new QGridLayout();
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(gerstnerOceanDataPtr->absDisplacement.x)), widget);
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, widget, [gerstnerOceanDataPtr](const QString& string)->void {
					gerstnerOceanDataPtr->absDisplacement.x = string.toFloat();
					Utils::Log::Message("absDisplacement.x: " + std::to_string(gerstnerOceanDataPtr->absDisplacement.x));
					});
				gridLayout->addWidget(lineEdit, 0, 0);
			}
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(gerstnerOceanDataPtr->absDisplacement.y)), widget);
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, widget, [gerstnerOceanDataPtr](const QString& string)->void {
					gerstnerOceanDataPtr->absDisplacement.y = string.toFloat();
					Utils::Log::Message("absDisplacement.y: " + std::to_string(gerstnerOceanDataPtr->absDisplacement.y));
					});
				gridLayout->addWidget(lineEdit, 0, 1);
			}
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(gerstnerOceanDataPtr->absDisplacement.z)), widget);
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, widget, [gerstnerOceanDataPtr](const QString& string)->void {
					gerstnerOceanDataPtr->absDisplacement.z = string.toFloat();
					Utils::Log::Message("absDisplacement.z: " + std::to_string(gerstnerOceanDataPtr->absDisplacement.z));
					});
				gridLayout->addWidget(lineEdit, 0, 2);
			}
			pLayout->addRow(QStringLiteral("absDisplacement: "), gridLayout);
		}

		// oceanScale
		{
			QGridLayout* gridLayout = new QGridLayout();
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(gerstnerOceanDataPtr->oceanScale.x)), widget);
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, widget, [gerstnerOceanDataPtr](const QString& string)->void {
					gerstnerOceanDataPtr->oceanScale.x = string.toFloat();
					Utils::Log::Message("oceanScale.x: " + std::to_string(gerstnerOceanDataPtr->oceanScale.x));
					});
				gridLayout->addWidget(lineEdit, 0, 0);
			}
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(gerstnerOceanDataPtr->oceanScale.y)), widget);
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, widget, [gerstnerOceanDataPtr](const QString& string)->void {
					gerstnerOceanDataPtr->oceanScale.y = string.toFloat();
					Utils::Log::Message("oceanScale.y: " + std::to_string(gerstnerOceanDataPtr->oceanScale.y));
					});
				gridLayout->addWidget(lineEdit, 0, 1);
			}
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(gerstnerOceanDataPtr->oceanScale.z)), widget);
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, widget, [gerstnerOceanDataPtr](const QString& string)->void {
					gerstnerOceanDataPtr->oceanScale.z = string.toFloat();
					Utils::Log::Message("oceanScale.z: " + std::to_string(gerstnerOceanDataPtr->oceanScale.z));
					});
				gridLayout->addWidget(lineEdit, 0, 2);
			}
			pLayout->addRow(QStringLiteral("oceanScale: "), gridLayout);
		}
	}

	// Aim Point
	{
		pLayout->addRow(QStringLiteral("-----Aim Point :-----"), new QWidget(widget));

		// aimPointDistanceFactor
		{
			QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(gerstnerOceanDataPtr->aimPointDistanceFactor)), widget);
			lineEdit->setValidator(doubleValidator);
			lineEdit->connect(lineEdit, &QLineEdit::textChanged, widget, [gerstnerOceanDataPtr](const QString& string)->void {
				gerstnerOceanDataPtr->aimPointDistanceFactor = string.toFloat();
				Utils::Log::Message("aimPointDistanceFactor: " + std::to_string(gerstnerOceanDataPtr->aimPointDistanceFactor));
				});
			pLayout->addRow(QStringLiteral("aimPointDistanceFactor: "), lineEdit);
		}
		// aimPointHeightCompensation
		{
			QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(gerstnerOceanDataPtr->aimPointHeightCompensation)), widget);
			lineEdit->setValidator(doubleValidator);
			lineEdit->connect(lineEdit, &QLineEdit::textChanged, widget, [gerstnerOceanDataPtr](const QString& string)->void {
				gerstnerOceanDataPtr->aimPointHeightCompensation = string.toFloat();
				Utils::Log::Message("aimPointHeightCompensation: " + std::to_string(gerstnerOceanDataPtr->aimPointHeightCompensation));
				});
			pLayout->addRow(QStringLiteral("aimPointHeightCompensation: "), lineEdit);
		}
	}

	// Render
	{
		pLayout->addRow(QStringLiteral("-----Render :-----"), new QWidget(widget));

		// showWireFrame
		{
			QCheckBox* checkBox = new QCheckBox(widget);
			checkBox->setCheckState(gerstnerOceanDataPtr->showWireFrame ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
			checkBox->connect(checkBox, &QCheckBox::stateChanged, widget, [gerstnerOceanDataPtr](int state)->void {
				gerstnerOceanDataPtr->showWireFrame = (state != Qt::CheckState::Unchecked);
				Utils::Log::Message(gerstnerOceanDataPtr->showWireFrame ? "showWireFrame: true" : "showWireFrame: false");
				});
			pLayout->addRow(QStringLiteral("showWireFrame: "), checkBox);
		}
	}

	// SubGerstnerWave
	{
		// size
		{
			QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(gerstnerOceanDataPtr->subGerstnerWaveInfos.size())), widget);
			lineEdit->setValidator(intValidator);
			lineEdit->connect(lineEdit, &QLineEdit::textChanged, widget, [gerstnerOceanDataPtr, pLayout, doubleValidator, this](const QString& string)->void {
				gerstnerOceanDataPtr->isDirty = true;

				const int oldSize = gerstnerOceanDataPtr->subGerstnerWaveInfos.size();
				const int newSize = string.toInt();

				if (newSize <= oldSize)
				{
					gerstnerOceanDataPtr->subGerstnerWaveInfos.resize(newSize);

					for (int i = oldSize - 1; newSize <= i; --i)
					{
						auto& subGerstnerWaveInfoWidgetVector = subGerstnerWaveInfoWidgetVectors.at(i);

						for (auto iter = subGerstnerWaveInfoWidgetVector.rbegin(); iter != subGerstnerWaveInfoWidgetVector.rend(); ++iter)
						{
							auto oldWidget = *iter;
							oldWidget->setParent(nullptr);
							pLayout->removeWidget(oldWidget);
							delete oldWidget;
						}
					}
					subGerstnerWaveInfoWidgetVectors.resize(newSize);
				}
				else
				{
					GerstnerOcean_RenderFeatureData::SubGerstnerWaveInfo newSubGerstnerWaveInfo{};
					newSubGerstnerWaveInfo.amplitudeFactor = 1;
					newSubGerstnerWaveInfo.windRotationAngle = 0;
					newSubGerstnerWaveInfo.waveLength = 1;
					newSubGerstnerWaveInfo.omegaFactor = 1;
					newSubGerstnerWaveInfo.phiAngle = 0;
					gerstnerOceanDataPtr->subGerstnerWaveInfos.resize(newSize, newSubGerstnerWaveInfo);

					subGerstnerWaveInfoWidgetVectors.resize(newSize, {});
					for (int i = oldSize; i < newSize; ++i)
					{
						auto& subGerstnerWaveInfoWidgetVector = subGerstnerWaveInfoWidgetVectors.at(i);
						auto subGerstnerWaveInfosPtr = &(gerstnerOceanDataPtr->subGerstnerWaveInfos);
						auto& subGerstnerWaveInfo = subGerstnerWaveInfosPtr->at(i);

						// amplitudeFactor
						{
							QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(subGerstnerWaveInfo.amplitudeFactor)), Widget());
							lineEdit->setValidator(doubleValidator);
							lineEdit->connect(lineEdit, &QLineEdit::textChanged, Widget(), [subGerstnerWaveInfosPtr, i, gerstnerOceanDataPtr](const QString& string)->void {
								auto& subGerstnerWaveInfo = subGerstnerWaveInfosPtr->at(i);
								gerstnerOceanDataPtr->isDirty = true;
								subGerstnerWaveInfo.amplitudeFactor = string.toFloat();
								Utils::Log::Message("amplitudeFactor: " + std::to_string(subGerstnerWaveInfo.amplitudeFactor));
								});
							pLayout->addRow(QStringLiteral("amplitudeFactor: "), lineEdit);
							subGerstnerWaveInfoWidgetVector.emplace_back(lineEdit);
						}

						// windRotationAngle
						{
							QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(subGerstnerWaveInfo.windRotationAngle)), Widget());
							lineEdit->setValidator(doubleValidator);
							lineEdit->connect(lineEdit, &QLineEdit::textChanged, Widget(), [subGerstnerWaveInfosPtr, i, gerstnerOceanDataPtr](const QString& string)->void {
								auto& subGerstnerWaveInfo = subGerstnerWaveInfosPtr->at(i);
								gerstnerOceanDataPtr->isDirty = true;
								subGerstnerWaveInfo.windRotationAngle = string.toFloat();
								Utils::Log::Message("windRotationAngle: " + std::to_string(subGerstnerWaveInfo.windRotationAngle));
								});
							pLayout->addRow(QStringLiteral("windRotationAngle: "), lineEdit);
							subGerstnerWaveInfoWidgetVector.emplace_back(lineEdit);
						}

						// waveLength
						{
							QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(subGerstnerWaveInfo.waveLength)), Widget());
							lineEdit->setValidator(doubleValidator);
							lineEdit->connect(lineEdit, &QLineEdit::textChanged, Widget(), [subGerstnerWaveInfosPtr, i, gerstnerOceanDataPtr](const QString& string)->void {
								auto& subGerstnerWaveInfo = subGerstnerWaveInfosPtr->at(i);
								gerstnerOceanDataPtr->isDirty = true;
								subGerstnerWaveInfo.waveLength = string.toFloat();
								Utils::Log::Message("waveLength: " + std::to_string(subGerstnerWaveInfo.waveLength));
								});
							pLayout->addRow(QStringLiteral("waveLength: "), lineEdit);
							subGerstnerWaveInfoWidgetVector.emplace_back(lineEdit);
						}

						// omegaFactor
						{
							QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(subGerstnerWaveInfo.omegaFactor)), Widget());
							lineEdit->setValidator(doubleValidator);
							lineEdit->connect(lineEdit, &QLineEdit::textChanged, Widget(), [subGerstnerWaveInfosPtr, i, gerstnerOceanDataPtr](const QString& string)->void {
								auto& subGerstnerWaveInfo = subGerstnerWaveInfosPtr->at(i);
								gerstnerOceanDataPtr->isDirty = true;
								subGerstnerWaveInfo.omegaFactor = string.toFloat();
								Utils::Log::Message("omegaFactor: " + std::to_string(subGerstnerWaveInfo.omegaFactor));
								});
							pLayout->addRow(QStringLiteral("omegaFactor: "), lineEdit);
							subGerstnerWaveInfoWidgetVector.emplace_back(lineEdit);
						}

						// phiAngle
						{
							QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(subGerstnerWaveInfo.phiAngle)), Widget());
							lineEdit->setValidator(doubleValidator);
							lineEdit->connect(lineEdit, &QLineEdit::textChanged, Widget(), [subGerstnerWaveInfosPtr, i, gerstnerOceanDataPtr](const QString& string)->void {
								auto& subGerstnerWaveInfo = subGerstnerWaveInfosPtr->at(i);
								gerstnerOceanDataPtr->isDirty = true;
								subGerstnerWaveInfo.phiAngle = string.toFloat();
								Utils::Log::Message("phiAngle: " + std::to_string(subGerstnerWaveInfo.phiAngle));
								});
							pLayout->addRow(QStringLiteral("phiAngle: "), lineEdit);
							subGerstnerWaveInfoWidgetVector.emplace_back(lineEdit);
						}
					}
				}
				Utils::Log::Message("size: " + std::to_string(gerstnerOceanDataPtr->aimPointHeightCompensation));
			});
			pLayout->addRow(QStringLiteral("size: "), lineEdit);	
		}

		// infos
		{
			const int i = 0;
			subGerstnerWaveInfoWidgetVectors.resize(1, {});
			auto& subGerstnerWaveInfoWidgetVector = subGerstnerWaveInfoWidgetVectors.at(i);
			auto subGerstnerWaveInfosPtr = &(gerstnerOceanDataPtr->subGerstnerWaveInfos);
			auto& subGerstnerWaveInfo = subGerstnerWaveInfosPtr->at(i);

			// amplitudeFactor
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(subGerstnerWaveInfo.amplitudeFactor)), Widget());
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, Widget(), [subGerstnerWaveInfosPtr, i, gerstnerOceanDataPtr](const QString& string)->void {
					auto& subGerstnerWaveInfo = subGerstnerWaveInfosPtr->at(i);
					gerstnerOceanDataPtr->isDirty = true;
					subGerstnerWaveInfo.amplitudeFactor = string.toFloat();
					Utils::Log::Message("amplitudeFactor: " + std::to_string(subGerstnerWaveInfo.amplitudeFactor));
					});
				pLayout->addRow(QStringLiteral("amplitudeFactor: "), lineEdit);
				subGerstnerWaveInfoWidgetVector.emplace_back(lineEdit);
			}

			// windRotationAngle
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(subGerstnerWaveInfo.windRotationAngle)), Widget());
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, Widget(), [subGerstnerWaveInfosPtr, i, gerstnerOceanDataPtr](const QString& string)->void {
					auto& subGerstnerWaveInfo = subGerstnerWaveInfosPtr->at(i);
					gerstnerOceanDataPtr->isDirty = true;
					subGerstnerWaveInfo.windRotationAngle = string.toFloat();
					Utils::Log::Message("windRotationAngle: " + std::to_string(subGerstnerWaveInfo.windRotationAngle));
					});
				pLayout->addRow(QStringLiteral("windRotationAngle: "), lineEdit);
				subGerstnerWaveInfoWidgetVector.emplace_back(lineEdit);
			}

			// waveLength
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(subGerstnerWaveInfo.waveLength)), Widget());
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, Widget(), [subGerstnerWaveInfosPtr, i, gerstnerOceanDataPtr](const QString& string)->void {
					auto& subGerstnerWaveInfo = subGerstnerWaveInfosPtr->at(i);
					gerstnerOceanDataPtr->isDirty = true;
					subGerstnerWaveInfo.waveLength = string.toFloat();
					Utils::Log::Message("waveLength: " + std::to_string(subGerstnerWaveInfo.waveLength));
					});
				pLayout->addRow(QStringLiteral("waveLength: "), lineEdit);
				subGerstnerWaveInfoWidgetVector.emplace_back(lineEdit);
			}

			// omegaFactor
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(subGerstnerWaveInfo.omegaFactor)), Widget());
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, Widget(), [subGerstnerWaveInfosPtr, i, gerstnerOceanDataPtr](const QString& string)->void {
					auto& subGerstnerWaveInfo = subGerstnerWaveInfosPtr->at(i);
					gerstnerOceanDataPtr->isDirty = true;
					subGerstnerWaveInfo.omegaFactor = string.toFloat();
					Utils::Log::Message("omegaFactor: " + std::to_string(subGerstnerWaveInfo.omegaFactor));
					});
				pLayout->addRow(QStringLiteral("omegaFactor: "), lineEdit);
				subGerstnerWaveInfoWidgetVector.emplace_back(lineEdit);
			}

			// phiAngle
			{
				QLineEdit* lineEdit = new QLineEdit(QString::fromStdString(std::to_string(subGerstnerWaveInfo.phiAngle)), Widget());
				lineEdit->setValidator(doubleValidator);
				lineEdit->connect(lineEdit, &QLineEdit::textChanged, Widget(), [subGerstnerWaveInfosPtr, i, gerstnerOceanDataPtr](const QString& string)->void {
					auto& subGerstnerWaveInfo = subGerstnerWaveInfosPtr->at(i);
					gerstnerOceanDataPtr->isDirty = true;
					subGerstnerWaveInfo.phiAngle = string.toFloat();
					Utils::Log::Message("phiAngle: " + std::to_string(subGerstnerWaveInfo.phiAngle));
					});
				pLayout->addRow(QStringLiteral("phiAngle: "), lineEdit);
				subGerstnerWaveInfoWidgetVector.emplace_back(lineEdit);
			}
		}

	}

	widget->setLayout(pLayout);
}
