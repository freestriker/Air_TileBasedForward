#include "Core/Graphic/Instance/NewImage.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Utils/Log.h"
#include "Core/Graphic/Instance/Memory.h"
#include "Core/Graphic/Manager/MemoryManager.h"
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

AirEngine::Core::Graphic::Instance::NewImage::NewImage()
	: AssetBase(true)
	, _isNative(true)
	, _imageImfo()
	, _vkImage(VK_NULL_HANDLE)
	, _memory(nullptr)
	, _vkExtent2D()
	, _imageViews()
	, _perLayerSize()
{
}

AirEngine::Core::Graphic::Instance::NewImage::~NewImage()
{
	if (_isNative)
	{
		for (auto& imageViewPair : _imageViews)
		{
			vkDestroyImageView(Core::Graphic::CoreObject::Instance::VkDevice_(), imageViewPair.second.vkImageView, nullptr);

		}
		vkDestroyImage(Core::Graphic::CoreObject::Instance::VkDevice_(), _vkImage, nullptr);
		Core::Graphic::CoreObject::Instance::MemoryManager().ReleaseMemory(*_memory);
		delete _memory;
	}
}

const AirEngine::Core::Graphic::Instance::NewImage::ImageView& AirEngine::Core::Graphic::Instance::NewImage::ImageView_(std::string imageViewName)
{
	return _imageViews[imageViewName];
}

VkImage AirEngine::Core::Graphic::Instance::NewImage::VkImage_()
{
	return _vkImage;
}

size_t AirEngine::Core::Graphic::Instance::NewImage::PerLayerSize()
{
	return _perLayerSize;
}

uint32_t AirEngine::Core::Graphic::Instance::NewImage::LayerCount()
{
	return static_cast<uint32_t>(_imageImfo.subresourcePaths.size());
}

VkExtent3D AirEngine::Core::Graphic::Instance::NewImage::VkExtent3D_()
{
	return VkExtent3D{ _vkExtent2D.width, _vkExtent2D .height, 1};
}

void AirEngine::Core::Graphic::Instance::NewImage::OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)
{
	//Load info
	{
		std::ifstream input_file(Path());
		Utils::Log::Exception("Failed to open image info file: " + Path() + " .", !input_file.is_open());
		nlohmann::json j = nlohmann::json::parse((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
		_imageImfo = j.get<ImageInfo>();
		input_file.close();
	}

	//Load bitmap
	std::vector<std::vector<unsigned char>> byteDatas;
	{
		bool inited = false;
		uint32_t pixelDepth = 0;
		uint32_t pitch = 0;
		VkExtent2D& extent2D = _vkExtent2D;

		for (size_t i = 0; i < _imageImfo.subresourcePaths.size(); i++)
		{
			auto subresourcePath = _imageImfo.subresourcePaths[i].c_str();
			auto fileType = FreeImage_GetFileType(subresourcePath);
			if (fileType == FREE_IMAGE_FORMAT::FIF_UNKNOWN) fileType = FreeImage_GetFIFFromFilename(subresourcePath);
			if ((fileType != FREE_IMAGE_FORMAT::FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fileType))
			{
				FIBITMAP* bitmap = FreeImage_Load(fileType, subresourcePath);

				uint32_t subPixelDepth = FreeImage_GetBPP(bitmap);
				uint32_t subPitch = FreeImage_GetPitch(bitmap);
				VkExtent2D subExtent2D = VkExtent2D{ FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap) };

				if (!inited)
				{
					pixelDepth = subPixelDepth;
					pitch = subPitch;
					extent2D = subExtent2D;
					_perLayerSize = static_cast<size_t>(extent2D.width) * extent2D.height * subPitch;
					byteDatas = std::vector<std::vector<unsigned char>>(_imageImfo.subresourcePaths.size(), std::vector<unsigned char>(_perLayerSize));
				}
				else
				{
					if (pixelDepth != subPixelDepth || pitch != subPitch || extent2D.width != subExtent2D.width || extent2D.height != subExtent2D.height)
					{
						Utils::Log::Exception("Failed to load subresource file: " + _imageImfo.subresourcePaths[i] + " , because of wrong file info.");
					}
				}
				FreeImage_ConvertToRawBits(byteDatas[i].data(), bitmap, subPitch, subPixelDepth, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, 0L);
				FreeImage_Unload(bitmap);
			}
			else
			{
				Utils::Log::Exception("Failed to open subresource file: " + _imageImfo.subresourcePaths[i] + " .");
			}
		}
	}

	//Create staging buffer
	Core::Graphic::Instance::Buffer stagingBuffer = Core::Graphic::Instance::Buffer(
		_perLayerSize * _imageImfo.subresourcePaths.size(), 
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	stagingBuffer.WriteData([this, &byteDatas](void* transferDst) {
		for (int i = 0; i < 6; i++)
		{
			memcpy(static_cast<char*>(transferDst) + i * _perLayerSize, byteDatas[i].data(), _perLayerSize);
		}
	});

	///Create vk image
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VkImageType::VK_IMAGE_TYPE_2D;
		imageInfo.extent = { _vkExtent2D.width, _vkExtent2D.height, 1 };
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = static_cast<uint32_t>(_imageImfo.subresourcePaths.size());
		imageInfo.format = _imageImfo.format;
		imageInfo.tiling = _imageImfo.imageTiling;
		imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = _imageImfo.imageUsageFlags;
		imageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags = _imageImfo.imageCreateFlags;

		Utils::Log::Exception("Failed to create image.", vkCreateImage(CoreObject::Instance::VkDevice_(), &imageInfo, nullptr, &_vkImage));

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(CoreObject::Instance::VkDevice_(), _vkImage, &memRequirements);

		auto newMemory = new Instance::Memory(CoreObject::Instance::MemoryManager().AcquireMemory(memRequirements, _imageImfo.memoryPropertyFlags));
		vkBindImageMemory(CoreObject::Instance::VkDevice_(), _vkImage, newMemory->VkDeviceMemory_(), newMemory->Offset());
	}

	///Create vk image view
	{
		for (auto& imageInfoPair : _imageImfo.imageViewInfos)
		{
			ImageView imageView = {};

			imageView.vkImageSubresourceRange.aspectMask = imageInfoPair.second.imageAspectFlags;
			imageView.vkImageSubresourceRange.baseMipLevel = 0;
			imageView.vkImageSubresourceRange.levelCount = 1;
			imageView.vkImageSubresourceRange.baseArrayLayer = imageInfoPair.second.baseArrayLayer;
			imageView.vkImageSubresourceRange.layerCount = imageInfoPair.second.layerCount;

			imageView.vkImageSubresourceLayers.aspectMask = imageInfoPair.second.imageAspectFlags;
			imageView.vkImageSubresourceLayers.mipLevel = 0;
			imageView.vkImageSubresourceLayers.baseArrayLayer = imageInfoPair.second.baseArrayLayer;
			imageView.vkImageSubresourceLayers.layerCount = imageInfoPair.second.layerCount;

			VkImageViewCreateInfo imageViewCreateInfo{};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = _vkImage;
			imageViewCreateInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE;
			imageViewCreateInfo.format = _imageImfo.format;
			imageViewCreateInfo.subresourceRange = imageView.vkImageSubresourceRange;

			Utils::Log::Exception("Failed to create image view.", vkCreateImageView(CoreObject::Instance::VkDevice_(), &imageViewCreateInfo, nullptr, &imageView.vkImageView));

			_imageViews.emplace(imageInfoPair.first, imageView);
		}
	}

	//Copy buffer to image
	{
		transferCommandBuffer->Reset();
		transferCommandBuffer->BeginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		Core::Graphic::Command::ImageMemoryBarrier imageTransferStartBarrier = Core::Graphic::Command::ImageMemoryBarrier(
			this,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			0,
			VK_ACCESS_TRANSFER_WRITE_BIT
		);
		transferCommandBuffer->AddPipelineImageBarrier(
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			{ &imageTransferStartBarrier }
		);
		transferCommandBuffer->CopyBufferToImage(&stagingBuffer, this, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		Core::Graphic::Command::ImageMemoryBarrier imageTransferEndBarrier = Core::Graphic::Command::ImageMemoryBarrier(
			this,
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
	}
}
