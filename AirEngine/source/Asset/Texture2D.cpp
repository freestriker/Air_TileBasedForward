#include "Asset/Texture2D.h"
#include "FreeImage/FreeImage.h"
#include <fstream>
#include "Utils/Log.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Command/Semaphore.h"
#include "Core/Graphic/Instance/Memory.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"

void AirEngine::Asset::Texture2D::OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)
{
	//Load settings
	{
		std::ifstream input_file(Path());
		Utils::Log::Exception("Failed to open texture2d file: " + Path() + " .", !input_file.is_open());
		std::string text = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
		nlohmann::json j = nlohmann::json::parse(text);
		_settings = j.get<Texture2DSettings>();
		input_file.close();
	}


	Core::Graphic::Command::Semaphore semaphore = Core::Graphic::Command::Semaphore();
	std::vector<unsigned char> byteData = std::vector<unsigned char>();

	//Load bitmap
	{
		auto p = _settings.imagePath.c_str();
		auto fileType = FreeImage_GetFileType(p);
		if (fileType == FREE_IMAGE_FORMAT::FIF_UNKNOWN) fileType = FreeImage_GetFIFFromFilename(p);
		if ((fileType != FREE_IMAGE_FORMAT::FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fileType))
		{
			FIBITMAP* bitmap = FreeImage_Load(fileType, p);
			uint32_t pixelDepth = FreeImage_GetBPP(bitmap);
			if (pixelDepth != 32)
			{
				Utils::Log::Exception("This texture2d asset is not 32 bit.");
			}
			uint32_t pitch = FreeImage_GetPitch(bitmap);
			_extent = VkExtent2D{ FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap) };
			_textureInfo.size = glm::vec4(_extent.width, _extent.height, 1.0 / _extent.width, 1.0 / _extent.height);
			_textureInfo.tilingScale = glm::vec4(0, 0, 1, 1);
			byteData.resize(static_cast<size_t>(_extent.width) * _extent.height * 4);
			FreeImage_ConvertToRawBits(byteData.data(), bitmap, pitch, pixelDepth, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, 0L);
			FreeImage_Unload(bitmap);
		}
	}
	//Create image
	{
		_image = Core::Graphic::Instance::Image::Create2DImage
		(
			_extent, 
			_settings.format, 
			VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | _settings.usage,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);

		if (_settings.useSampler)
		{
			_imageSampler = new Core::Graphic::Instance::ImageSampler
			(
				_settings.magFilter,
				VK_SAMPLER_MIPMAP_MODE_LINEAR,
				_settings.addressMode,
				_settings.anisotropy,
				_settings.borderColor
			);
		}
	}

	//Create buffer
	Core::Graphic::Instance::Buffer textureStagingBuffer = Core::Graphic::Instance::Buffer(static_cast<uint64_t>(_extent.width) * _extent.height * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	textureStagingBuffer.WriteData(byteData.data(), textureStagingBuffer.Size());

	_textureInfoBuffer = new Core::Graphic::Instance::Buffer(sizeof(_textureInfo), VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	_textureInfoBuffer->WriteData(&_textureInfo, sizeof(_textureInfo));

	transferCommandBuffer->Reset();
	transferCommandBuffer->BeginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	Core::Graphic::Command::ImageMemoryBarrier imageTransferStartBarrier = Core::Graphic::Command::ImageMemoryBarrier(
		_image,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		0,
		VK_ACCESS_TRANSFER_WRITE_BIT
	);
	transferCommandBuffer->AddPipelineImageBarrier(
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		{ &imageTransferStartBarrier }
	);

	transferCommandBuffer->CopyBufferToImage(&textureStagingBuffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	Core::Graphic::Command::ImageMemoryBarrier imageTransferEndBarrier = Core::Graphic::Command::ImageMemoryBarrier(
		_image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		0
	);
	transferCommandBuffer->AddPipelineImageBarrier(
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		{ &imageTransferEndBarrier }
	);

	transferCommandBuffer->EndRecord();
	transferCommandBuffer->Submit();
	transferCommandBuffer->WaitForFinish();
	transferCommandBuffer->Reset();
}

AirEngine::Asset::Texture2D::Texture2D()
	: AssetBase(true)
	, _extent()
	, _textureInfoBuffer(nullptr)
	, _image(nullptr)
	, _imageSampler(nullptr)
	, _textureInfo()
	, _settings()
{
}

AirEngine::Asset::Texture2D::Texture2D(VkExtent2D extent, Texture2DSettings settings)
	: AssetBase(false)
	, _extent(extent)
	, _textureInfoBuffer(nullptr)
	, _image(nullptr)
	, _imageSampler(nullptr)
	, _textureInfo()
	, _settings(settings)
{
	_textureInfo.size = glm::vec4(_extent.width, _extent.height, 1.0 / _extent.width, 1.0 / _extent.height);
	_textureInfo.tilingScale = glm::vec4(0, 0, 1, 1);

	//Create image
	{
		_image = Core::Graphic::Instance::Image::Create2DImage
		(
			_extent,
			_settings.format,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | _settings.usage,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageTiling::VK_IMAGE_TILING_OPTIMAL
		);

		if (_settings.useSampler)
		{
			_imageSampler = new Core::Graphic::Instance::ImageSampler
			(
				_settings.magFilter,
				VK_SAMPLER_MIPMAP_MODE_LINEAR,
				_settings.addressMode,
				_settings.anisotropy,
				_settings.borderColor
			);
		}
	}

	//Create buffer
	_textureInfoBuffer = new Core::Graphic::Instance::Buffer(sizeof(_textureInfo), VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	_textureInfoBuffer->WriteData(&_textureInfo, sizeof(_textureInfo));
}

AirEngine::Asset::Texture2D::~Texture2D()
{
	delete _textureInfoBuffer;

	delete _imageSampler;

	delete _image;
}

void AirEngine::Asset::Texture2D::WriteData(Core::Graphic::Command::CommandBuffer* transferCommandBuffer, std::vector<unsigned char>& byteData)
{
	Core::Graphic::Instance::Buffer textureStagingBuffer = Core::Graphic::Instance::Buffer(static_cast<uint64_t>(_extent.width) * _extent.height * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	textureStagingBuffer.WriteData(byteData.data(), textureStagingBuffer.Size());
	
	transferCommandBuffer->Reset();
	transferCommandBuffer->BeginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	Core::Graphic::Command::ImageMemoryBarrier imageTransferStartBarrier = Core::Graphic::Command::ImageMemoryBarrier(
		_image,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		0,
		VK_ACCESS_TRANSFER_WRITE_BIT
	);
	transferCommandBuffer->AddPipelineImageBarrier(
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
		{ &imageTransferStartBarrier }
	);

	transferCommandBuffer->CopyBufferToImage(&textureStagingBuffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	Core::Graphic::Command::ImageMemoryBarrier imageTransferEndBarrier = Core::Graphic::Command::ImageMemoryBarrier(
		_image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		0
	);
	transferCommandBuffer->AddPipelineImageBarrier(
		VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		{ &imageTransferEndBarrier }
	);

	transferCommandBuffer->EndRecord();
	transferCommandBuffer->Submit();
	transferCommandBuffer->WaitForFinish();
	transferCommandBuffer->Reset();
}

VkExtent2D AirEngine::Asset::Texture2D::VkExtent2D_()
{
	return _extent;
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Asset::Texture2D::InfoBuffer()
{
	return _textureInfoBuffer;
}

AirEngine::Core::Graphic::Instance::Image* AirEngine::Asset::Texture2D::Image()
{
	return _image;
}

AirEngine::Core::Graphic::Instance::ImageSampler* AirEngine::Asset::Texture2D::ImageSampler()
{
	return _imageSampler;
}

AirEngine::Asset::Texture2D::TextureInfo AirEngine::Asset::Texture2D::Info()
{
	return _textureInfo;
}

AirEngine::Asset::Texture2D::Texture2DSettings AirEngine::Asset::Texture2D::Settings()
{
	return _settings;
}
