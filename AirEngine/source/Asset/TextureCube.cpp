#include "Asset/TextureCube.h"
#include "FreeImage/FreeImage.h"
#include "Core/Graphic/Command/CommandBuffer.h"
#include "Core/Graphic/Instance/Buffer.h"
#include <Utils/Log.h>
#include "Core/Graphic/Command/Semaphore.h"
#include "Core/Graphic/Instance/Memory.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/Command/ImageMemoryBarrier.h"
#include <fstream>

void AirEngine::Asset::TextureCube::OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)
{
	//Load settings from json file
	{
		std::ifstream input_file(Path());
		Utils::Log::Exception("Failed to open texturecube file.", !input_file.is_open());
		std::string text = std::string((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
		nlohmann::json j = nlohmann::json::parse(text);
		_settings = j.get<TextureCubeSetting>();
		input_file.close();
	}

	//Load bitmap
	size_t perFaceSize = 0;
	{
		for (size_t i = 0; i < 6; i++)
		{
			auto texturePath = _settings.paths[i].c_str();
			auto fileType = FreeImage_GetFileType(texturePath);
			if (fileType == FREE_IMAGE_FORMAT::FIF_UNKNOWN) fileType = FreeImage_GetFIFFromFilename(texturePath);
			if ((fileType != FREE_IMAGE_FORMAT::FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fileType))
			{
				FIBITMAP* bitmap = FreeImage_Load(fileType, texturePath);
				uint32_t pixelDepth = FreeImage_GetBPP(bitmap);
				if (pixelDepth != 32)
				{
					Utils::Log::Exception("This texturecube asset is not 32 bit.");
				}
				uint32_t pitch = FreeImage_GetPitch(bitmap);
				_extent = VkExtent2D{ FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap) };
				perFaceSize = static_cast<size_t>(_extent.width) * _extent.height * 4;
				_byteDatas[i].resize(perFaceSize);
				FreeImage_ConvertToRawBits(_byteDatas[i].data(), bitmap, pitch, pixelDepth, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, 0L);
				FreeImage_Unload(bitmap);
			}
		}
	}

	//Create image
	{
		_image = Core::Graphic::Instance::Image::CreateCubeImage
		(
			_extent,
			_settings.format,
			VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT,
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT
		);

		_imageSampler = new Core::Graphic::Instance::ImageSampler(
			_settings.filter,
			_settings.mipmapMode,
			_settings.addressMode,
			_settings.anisotropy,
			_settings.borderColor
		);
	}

	//Create staging buffer
	Core::Graphic::Instance::Buffer stagingBuffer = Core::Graphic::Instance::Buffer(static_cast<uint64_t>(perFaceSize * 6), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	stagingBuffer.WriteData([perFaceSize, this](void* transferDst) {
		for (int i = 0; i < 6; i++)
		{
			memcpy(static_cast<char*>(transferDst) + i * perFaceSize, _byteDatas[i].data(), perFaceSize);
		}
	});

	//Copy buffer to image
	{
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
		transferCommandBuffer->CopyBufferToImage(&stagingBuffer, _image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
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
}

AirEngine::Asset::TextureCube::TextureCube()
	: AssetBase(true)
	, _extent()
	, _image(nullptr)
	, _imageSampler(nullptr)
	, _byteDatas()
	, _settings()
{
}

AirEngine::Asset::TextureCube::TextureCube(Core::Graphic::Command::CommandBuffer* transferCommandBuffer, VkExtent2D extent, TextureCubeSetting settings, std::array<std::vector<unsigned char>, 6>& byteDatas)
	: AssetBase(false)
	, _extent(extent)
	, _image(nullptr)
	, _imageSampler(nullptr)
	, _byteDatas(byteDatas)
	, _settings(settings)
{
}

AirEngine::Asset::TextureCube::~TextureCube()
{
	delete _imageSampler;

	delete _image;
}

VkExtent2D AirEngine::Asset::TextureCube::VkExtent2D_()
{
	return _extent;
}

AirEngine::Core::Graphic::Instance::Image* AirEngine::Asset::TextureCube::Image()
{
	return _image;
}

AirEngine::Core::Graphic::Instance::ImageSampler* AirEngine::Asset::TextureCube::ImageSampler()
{
	return _imageSampler;
}

std::array<std::vector<unsigned char>, 6>* AirEngine::Asset::TextureCube::ByteData()
{
	return &_byteDatas;
}

AirEngine::Asset::TextureCube::TextureCubeSetting AirEngine::Asset::TextureCube::Settings()
{
	return _settings;
}
