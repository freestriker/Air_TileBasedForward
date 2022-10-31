#include "Core/Graphic/Instance/Image.h"
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

AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Instance::Image::Create2DImage(VkExtent2D extent, VkFormat format, VkImageUsageFlags imageUsage, VkMemoryPropertyFlags memoryProperty, VkImageAspectFlags aspect, VkImageTiling imageTiling)
{
	auto newImage = new Image();

	newImage->_vkExtent2D = extent;
	newImage->_isNative = false;
	newImage->_layerCount = 1;
	newImage->_imageImfo.subresourcePaths = { };
	newImage->_imageImfo.format = format;
	newImage->_imageImfo.imageTiling = imageTiling;
	newImage->_imageImfo.imageUsageFlags = imageUsage;
	newImage->_imageImfo.memoryPropertyFlags = memoryProperty;
	newImage->_imageImfo.imageCreateFlags = 0;
	newImage->_imageImfo.imageViewInfos = {
		{
			"DefaultImageView",
			{
				VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
				aspect,
				0,
				1
			}
		}
	};

	newImage->CreateVulkanInstance();

	return newImage;
}

AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Instance::Image::CreateNative2DImage(VkImage vkImage, VkImageView vkImageView, VkExtent2D extent, VkFormat format, VkImageUsageFlags imageUsage, VkImageAspectFlags aspect)
{
	auto newImage = new Image();

	newImage->_vkExtent2D = extent;
	newImage->_isNative = true;
	newImage->_layerCount = 1;
	newImage->_imageImfo.subresourcePaths = { };
	newImage->_imageImfo.format = format;
	newImage->_imageImfo.imageTiling = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
	newImage->_imageImfo.imageUsageFlags = imageUsage;
	newImage->_imageImfo.memoryPropertyFlags = 0;
	newImage->_imageImfo.imageCreateFlags = 0;
	newImage->_imageImfo.imageViewInfos = {
		{
			"DefaultImageView",
			{
				VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
				aspect,
				0,
				1
			}
		}
	};

	newImage->_vkImage = vkImage;

	ImageView imageView = {};

	imageView.vkImageSubresourceRange.aspectMask = aspect;
	imageView.vkImageSubresourceRange.baseMipLevel = 0;
	imageView.vkImageSubresourceRange.levelCount = 1;
	imageView.vkImageSubresourceRange.baseArrayLayer = 0;
	imageView.vkImageSubresourceRange.layerCount = 1;

	imageView.vkImageSubresourceLayers.aspectMask = aspect;
	imageView.vkImageSubresourceLayers.mipLevel = 0;
	imageView.vkImageSubresourceLayers.baseArrayLayer = 0;
	imageView.vkImageSubresourceLayers.layerCount = 1;

	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = vkImage;
	imageViewCreateInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.subresourceRange = imageView.vkImageSubresourceRange;

	Utils::Log::Exception("Failed to create image view.", vkCreateImageView(CoreObject::Instance::VkDevice_(), &imageViewCreateInfo, nullptr, &imageView.vkImageView));

	newImage->_imageViews.emplace("DefaultImageView", imageView);

	return newImage;
}

AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Instance::Image::Create2DImageArray(VkExtent2D extent, VkFormat format, VkImageUsageFlags imageUsage, VkMemoryPropertyFlags memoryProperty, VkImageAspectFlags aspect, uint32_t arraySize, VkImageTiling imageTiling)
{
	auto newImage = new Image();

	newImage->_vkExtent2D = extent;
	newImage->_isNative = false;
	newImage->_layerCount = arraySize;
	newImage->_imageImfo.subresourcePaths = { };
	newImage->_imageImfo.format = format;
	newImage->_imageImfo.imageTiling = imageTiling;
	newImage->_imageImfo.imageUsageFlags = imageUsage;
	newImage->_imageImfo.memoryPropertyFlags = memoryProperty;
	newImage->_imageImfo.imageCreateFlags = 0;
	newImage->_imageImfo.imageViewInfos = {
		{
			"DefaultImageView",
			{
				VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY,
				aspect,
				0,
				arraySize
			}
		}
	};

	newImage->CreateVulkanInstance();

	return newImage;
}

AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Instance::Image::CreateCubeImage(VkExtent2D extent, VkFormat format, VkImageUsageFlags imageUsage, VkMemoryPropertyFlags memoryProperty, VkImageAspectFlags aspect, VkImageTiling imageTiling)
{
	auto newImage = new Image();

	newImage->_vkExtent2D = extent;
	newImage->_isNative = false;
	newImage->_layerCount = 6;
	newImage->_imageImfo.subresourcePaths = { };
	newImage->_imageImfo.format = format;
	newImage->_imageImfo.imageTiling = imageTiling;
	newImage->_imageImfo.imageUsageFlags = imageUsage;
	newImage->_imageImfo.memoryPropertyFlags = memoryProperty;
	newImage->_imageImfo.imageCreateFlags = VkImageCreateFlagBits::VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	newImage->_imageImfo.imageViewInfos = {
		{
			"DefaultImageView",
			{
				VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
				aspect,
				0,
				6
			}
		}
	};

	newImage->CreateVulkanInstance();

	return newImage;
}

AirEngine::Core::Graphic::Instance::Image::Image()
	: AssetBase(true)
	, _isNative(false)
	, _imageImfo()
	, _vkImage(VK_NULL_HANDLE)
	, _memory(nullptr)
	, _vkExtent2D()
	, _imageViews()
	, _layerCount()
{
}

AirEngine::Core::Graphic::Instance::Image::~Image()
{
	if (!_isNative)
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

void AirEngine::Core::Graphic::Instance::Image::AddImageView(std::string name, VkImageViewType imageViewType, VkImageAspectFlags imageAspectFlags, uint32_t baseArrayLayer, uint32_t layerCount)
{
	ImageViewInfo imageViewInfo{};
	imageViewInfo.imageViewType = imageViewType;
	imageViewInfo.imageAspectFlags = imageAspectFlags;
	imageViewInfo.baseArrayLayer = baseArrayLayer;
	imageViewInfo.layerCount = layerCount;

	_imageImfo.imageViewInfos.emplace(name, imageViewInfo);

	ImageView imageView = {};

	imageView.vkImageSubresourceRange.aspectMask = imageAspectFlags;
	imageView.vkImageSubresourceRange.baseMipLevel = 0;
	imageView.vkImageSubresourceRange.levelCount = 1;
	imageView.vkImageSubresourceRange.baseArrayLayer = baseArrayLayer;
	imageView.vkImageSubresourceRange.layerCount = layerCount;

	imageView.vkImageSubresourceLayers.aspectMask = imageAspectFlags;
	imageView.vkImageSubresourceLayers.mipLevel = 0;
	imageView.vkImageSubresourceLayers.baseArrayLayer = baseArrayLayer;
	imageView.vkImageSubresourceLayers.layerCount = layerCount;

	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = _vkImage;
	imageViewCreateInfo.viewType = VkImageViewType::VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = _imageImfo.format;
	imageViewCreateInfo.subresourceRange = imageView.vkImageSubresourceRange;

	Utils::Log::Exception("Failed to create image view.", vkCreateImageView(CoreObject::Instance::VkDevice_(), &imageViewCreateInfo, nullptr, &imageView.vkImageView));

	_imageViews.emplace(name, imageView);
}

void AirEngine::Core::Graphic::Instance::Image::RemoveImageView(std::string name)
{
	vkDestroyImageView(Core::Graphic::CoreObject::Instance::VkDevice_(), _imageViews[name].vkImageView, nullptr);
	_imageViews.erase(name);
	_imageImfo.imageViewInfos.erase(name);
}

const AirEngine::Core::Graphic::Instance::Image::ImageView& AirEngine::Core::Graphic::Instance::Image::ImageView_(std::string imageViewName)
{
	return _imageViews[imageViewName];
}

VkImage AirEngine::Core::Graphic::Instance::Image::VkImage_()
{
	return _vkImage;
}

uint32_t AirEngine::Core::Graphic::Instance::Image::LayerCount()
{
	return _layerCount;
}

VkFormat AirEngine::Core::Graphic::Instance::Image::VkFormat_()
{
	return _imageImfo.format;
}

VkMemoryPropertyFlags AirEngine::Core::Graphic::Instance::Image::VkMemoryPropertyFlags_()
{
	return _imageImfo.memoryPropertyFlags;
}

AirEngine::Core::Graphic::Instance::Memory& AirEngine::Core::Graphic::Instance::Image::Memory_()
{
	return *_memory;
}

VkExtent3D AirEngine::Core::Graphic::Instance::Image::VkExtent3D_()
{
	return VkExtent3D{ _vkExtent2D.width, _vkExtent2D .height, 1};
}

VkExtent2D AirEngine::Core::Graphic::Instance::Image::VkExtent2D_()
{
	return _vkExtent2D;
}

void AirEngine::Core::Graphic::Instance::Image::OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer)
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
	size_t perLayerSize = 0;
	{
		bool inited = false;
		uint32_t pixelDepth = 0;
		uint32_t pitch = 0;
		VkExtent2D& extent2D = _vkExtent2D;
		uint32_t& layerCount = _layerCount;

		for (size_t i = 0; i < _imageImfo.subresourcePaths.size(); i++)
		{
			auto subresourcePath = _imageImfo.subresourcePaths[i].c_str();
			auto fileType = FreeImage_GetFileType(subresourcePath);
			if (fileType == FREE_IMAGE_FORMAT::FIF_UNKNOWN) fileType = FreeImage_GetFIFFromFilename(subresourcePath);
			if ((fileType != FREE_IMAGE_FORMAT::FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fileType))
			{
				FIBITMAP* bitmap = FreeImage_Load(fileType, subresourcePath);
				if (_imageImfo.targetType != FREE_IMAGE_TYPE::FIT_UNKNOWN)
				{
					FIBITMAP* newBitmap = FreeImage_ConvertToType(bitmap, _imageImfo.targetType);
					FreeImage_Unload(bitmap);
					bitmap = newBitmap;
				}

				uint32_t subPixelDepth = FreeImage_GetBPP(bitmap);
				uint32_t subPitch = FreeImage_GetPitch(bitmap);
				VkExtent2D subExtent2D = VkExtent2D{ FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap) };

				if (!inited)
				{
					inited = true;

					pixelDepth = subPixelDepth;
					pitch = subPitch;
					extent2D = subExtent2D;
					perLayerSize = static_cast<size_t>(extent2D.width) * extent2D.height * subPixelDepth / 8;
					byteDatas = std::vector<std::vector<unsigned char>>(_imageImfo.subresourcePaths.size(), std::vector<unsigned char>(perLayerSize));
					layerCount = static_cast<uint32_t>(_imageImfo.subresourcePaths.size());
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

	CreateVulkanInstance();

	//Create staging buffer
	Core::Graphic::Instance::Buffer stagingBuffer = Core::Graphic::Instance::Buffer(
		perLayerSize * _layerCount,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	);
	stagingBuffer.WriteData([perLayerSize, &byteDatas](void* transferDst) {
		for (int i = 0; i < byteDatas.size(); i++)
		{
			memcpy(static_cast<char*>(transferDst) + i * perLayerSize, byteDatas[i].data(), perLayerSize);
		}
	});


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

void AirEngine::Core::Graphic::Instance::Image::CreateVulkanInstance()
{
	///Create vk image
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VkImageType::VK_IMAGE_TYPE_2D;
		imageInfo.extent = { _vkExtent2D.width, _vkExtent2D.height, 1 };
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = _layerCount;
		imageInfo.format = _imageImfo.format;
		imageInfo.tiling = _imageImfo.imageTiling;
		imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = _imageImfo.imageUsageFlags | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		imageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.flags = _imageImfo.imageCreateFlags;

		Utils::Log::Exception("Failed to create image.", vkCreateImage(CoreObject::Instance::VkDevice_(), &imageInfo, nullptr, &_vkImage));

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(CoreObject::Instance::VkDevice_(), _vkImage, &memRequirements);

		_memory = new Instance::Memory(CoreObject::Instance::MemoryManager().AcquireMemory(memRequirements, _imageImfo.memoryPropertyFlags));
		vkBindImageMemory(CoreObject::Instance::VkDevice_(), _vkImage, _memory->VkDeviceMemory_(), _memory->Offset());
	}

	///Create vk image view
	{
		if (_imageImfo.imageViewInfos.find("DefaultImageView") == _imageImfo.imageViewInfos.end())
		{
			Utils::Log::Exception("Failed to find DefaultImageView.");
		}

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
			imageViewCreateInfo.viewType = imageInfoPair.second.imageViewType;
			imageViewCreateInfo.format = _imageImfo.format;
			imageViewCreateInfo.subresourceRange = imageView.vkImageSubresourceRange;

			Utils::Log::Exception("Failed to create image view.", vkCreateImageView(CoreObject::Instance::VkDevice_(), &imageViewCreateInfo, nullptr, &imageView.vkImageView));

			_imageViews.emplace(imageInfoPair.first, imageView);
		}
	}
}
