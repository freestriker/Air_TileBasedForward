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
#include <glm/glm.hpp>

AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Instance::Image::Create2DImage(VkExtent2D extent, VkFormat format, VkImageUsageFlags imageUsage, VkMemoryPropertyFlags memoryProperty, VkImageAspectFlags aspect, VkImageTiling imageTiling)
{
	auto newImage = new Image();

	newImage->_vkExtent2D = extent;
	newImage->_vkExtent2Ds.push_back(extent);
	newImage->_isNative = false;
	newImage->_layerCount = 1;
	newImage->_mipmapLevelCount = 1;
	newImage->_imageImfo.mipmapLayerSourcePaths = { };
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
				1,
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
	newImage->_vkExtent2Ds.push_back(extent);
	newImage->_isNative = true;
	newImage->_layerCount = 1;
	newImage->_mipmapLevelCount = 1;
	newImage->_imageImfo.mipmapLayerSourcePaths = { };
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
				1,
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

	imageView.vkExtent2Ds = &newImage->_vkExtent2Ds;

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
	newImage->_vkExtent2Ds.push_back(extent);
	newImage->_isNative = false;
	newImage->_layerCount = arraySize;
	newImage->_mipmapLevelCount = 1;
	newImage->_imageImfo.mipmapLayerSourcePaths = { };
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
				arraySize,
				0,
				1
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
	newImage->_vkExtent2Ds.push_back(extent);
	newImage->_isNative = false;
	newImage->_layerCount = 6;
	newImage->_mipmapLevelCount = 1;
	newImage->_imageImfo.mipmapLayerSourcePaths = { };
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
				6,
				0,
				1
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

void AirEngine::Core::Graphic::Instance::Image::AddImageView(std::string name, VkImageViewType imageViewType, VkImageAspectFlags imageAspectFlags, uint32_t baseArrayLayer, uint32_t layerCount, uint32_t baseMipmapLevel, uint32_t mipmapLevelCount)
{
	ImageViewInfo imageViewInfo{};
	imageViewInfo.imageViewType = imageViewType;
	imageViewInfo.imageAspectFlags = imageAspectFlags;
	imageViewInfo.baseLayer = baseArrayLayer;
	imageViewInfo.layerCount = layerCount;
	imageViewInfo.baseMipmapLevel = baseMipmapLevel;
	imageViewInfo.mipmapLevelCount = mipmapLevelCount;

	_imageImfo.imageViewInfos.emplace(name, imageViewInfo);

	ImageView imageView = {};

	imageView.vkImageSubresourceRange.aspectMask = imageAspectFlags;
	imageView.vkImageSubresourceRange.baseMipLevel = baseMipmapLevel;
	imageView.vkImageSubresourceRange.levelCount = mipmapLevelCount;
	imageView.vkImageSubresourceRange.baseArrayLayer = baseArrayLayer;
	imageView.vkImageSubresourceRange.layerCount = layerCount;

	imageView.vkExtent2Ds = &_vkExtent2Ds;

	VkImageViewCreateInfo imageViewCreateInfo{};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = _vkImage;
	imageViewCreateInfo.viewType = imageViewType;
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

AirEngine::Core::Graphic::Instance::Image::ImageView& AirEngine::Core::Graphic::Instance::Image::ImageView_(std::string imageViewName)
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

VkImageUsageFlags AirEngine::Core::Graphic::Instance::Image::VkImageUsageFlags_()
{
	return _imageImfo.imageUsageFlags;
}

VkMemoryPropertyFlags AirEngine::Core::Graphic::Instance::Image::VkMemoryPropertyFlags_()
{
	return _imageImfo.memoryPropertyFlags;
}

VkImageTiling AirEngine::Core::Graphic::Instance::Image::VkImageTiling_()
{
	return _imageImfo.imageTiling;
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

	///Pre load
	uint32_t needStagingLevelCount = 0;
	{
		needStagingLevelCount = static_cast<uint32_t>(_imageImfo.mipmapLayerSourcePaths.size());
		_layerCount = static_cast<uint32_t>(_imageImfo.mipmapLayerSourcePaths[0].size());
	}
	
	//Load bitmap
	
	std::vector<BYTE*> layerByteDatas(needStagingLevelCount, nullptr);
	std::vector<size_t> perLayerSizes(needStagingLevelCount, 0);
	std::vector<VkExtent2D> perLayerExtents(needStagingLevelCount, {0, 0});
	{
		for (int levelIndex = 0; levelIndex < needStagingLevelCount; levelIndex++)
		{
			uint32_t pixelDepth = 0;
			uint32_t pitch = 0;
			auto& perLayerExtent = perLayerExtents[levelIndex];
			auto& perLayerSize = perLayerSizes[levelIndex];
			auto& allLayerByteData = layerByteDatas[levelIndex];

			for (int layerIndex = 0; layerIndex < _layerCount; layerIndex++)
			{
				auto path = _imageImfo.mipmapLayerSourcePaths[levelIndex][layerIndex].c_str();

				auto fileType = FreeImage_GetFileType(path);
				if (fileType == FREE_IMAGE_FORMAT::FIF_UNKNOWN) fileType = FreeImage_GetFIFFromFilename(path);
				if ((fileType != FREE_IMAGE_FORMAT::FIF_UNKNOWN) && FreeImage_FIFSupportsReading(fileType))
				{
					FIBITMAP* bitmap = FreeImage_Load(fileType, path);
					if (_imageImfo.targetType != FREE_IMAGE_TYPE::FIT_UNKNOWN)
					{
						FIBITMAP* newBitmap = FreeImage_ConvertToType(bitmap, _imageImfo.targetType, FALSE);
						FreeImage_Unload(bitmap);
						bitmap = newBitmap;
					}

					if (layerIndex == 0)
					{
						pixelDepth = FreeImage_GetBPP(bitmap);
						pitch = FreeImage_GetPitch(bitmap);
						perLayerExtent = VkExtent2D{FreeImage_GetWidth(bitmap), FreeImage_GetHeight(bitmap)};
						perLayerSize = static_cast<size_t>(perLayerExtent.width) * perLayerExtent.height * pixelDepth / 8;
						allLayerByteData = static_cast<BYTE*>(std::malloc(perLayerSize * _layerCount));
					}

					FreeImage_ConvertToRawBits(allLayerByteData + perLayerSize * layerIndex, bitmap, pitch, pixelDepth, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, _imageImfo.topDown ? TRUE : FALSE);
					FreeImage_Unload(bitmap);
				}
				else
				{
					Utils::Log::Exception("Failed to open subresource file: " + _imageImfo.mipmapLayerSourcePaths[levelIndex][layerIndex] + " .");
				}
			}
		}
	}

	///Late load
	std::vector< Core::Graphic::Instance::Buffer*> perLevelStagingBufffers(needStagingLevelCount, nullptr);
	{
		_vkExtent2D = perLayerExtents[0];
		if (_imageImfo.autoGenerateMipmap)
		{
			_mipmapLevelCount = static_cast<uint32_t>(std::floor(std::log2(std::max(_vkExtent2D.width, _vkExtent2D.height)))) + 1;
		}
		else
		{
			_mipmapLevelCount = needStagingLevelCount;
		}
		_vkExtent2Ds.resize(_mipmapLevelCount);
		_vkExtent2Ds[0] = perLayerExtents[0];
		for (int levelIndex = 1; levelIndex < _mipmapLevelCount; levelIndex++)
		{
			_vkExtent2Ds[levelIndex].width = _vkExtent2Ds[levelIndex - 1].width / 2 < 1 ? 1 : _vkExtent2Ds[levelIndex - 1].width / 2;
			_vkExtent2Ds[levelIndex].height = _vkExtent2Ds[levelIndex - 1].height / 2 < 1 ? 1 : _vkExtent2Ds[levelIndex - 1].height / 2;
		}

		//Create staging buffer
		for (int levelIndex = 0; levelIndex < needStagingLevelCount; levelIndex++)
		{
			perLevelStagingBufffers[levelIndex] = new Core::Graphic::Instance::Buffer(
				perLayerSizes[levelIndex] * _layerCount,
				VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			);
			perLevelStagingBufffers[levelIndex]->WriteData(layerByteDatas[levelIndex], perLayerSizes[levelIndex] * _layerCount);
			delete layerByteDatas[levelIndex];
		}
	}

	///Create instance
	{
		CreateVulkanInstance();
		AddImageView(
			"Transfer_AllLevel",
			_imageImfo.imageViewInfos["DefaultImageView"].imageViewType,
			ImageView_().VkImageAspectFlags_(),
			0,
			_layerCount,
			0,
			_mipmapLevelCount
		);
		for (int levelIndex = 0; levelIndex < _mipmapLevelCount; levelIndex++)
		{
			AddImageView(
				"Transfer_Level" + std::to_string(levelIndex),
				_imageImfo.imageViewInfos["DefaultImageView"].imageViewType,
				ImageView_().VkImageAspectFlags_(),
				0,
				_layerCount,
				levelIndex,
				1
			);
		}
	}

	//Copy buffer to image
	{
		transferCommandBuffer->Reset();
		transferCommandBuffer->BeginRecord(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
		{
			Core::Graphic::Command::ImageMemoryBarrier barrier = Core::Graphic::Command::ImageMemoryBarrier(
				this,
				"Transfer_AllLevel",
				VK_IMAGE_LAYOUT_UNDEFINED,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				0,
				VK_ACCESS_TRANSFER_WRITE_BIT
			);
			transferCommandBuffer->AddPipelineImageBarrier(
				VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
				{ &barrier }
			);
		}
		for (int levelIndex = 0; levelIndex < needStagingLevelCount; levelIndex++)
		{
			transferCommandBuffer->CopyBufferToImage(perLevelStagingBufffers[levelIndex], this, "Transfer_Level" + std::to_string(levelIndex), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		}
		for (int generateLevelIndex = needStagingLevelCount; generateLevelIndex < _mipmapLevelCount; generateLevelIndex++)
		{
			{
				Core::Graphic::Command::ImageMemoryBarrier barrier = Core::Graphic::Command::ImageMemoryBarrier(
					this,
					"Transfer_Level" + std::to_string(generateLevelIndex - 1),
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					VK_ACCESS_TRANSFER_WRITE_BIT,
					VK_ACCESS_TRANSFER_READ_BIT
				);
				transferCommandBuffer->AddPipelineImageBarrier(
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &barrier }
				);
			}
			transferCommandBuffer->Blit(
				this, "Transfer_Level" + std::to_string(generateLevelIndex - 1), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				this, "Transfer_Level" + std::to_string(generateLevelIndex), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_FILTER_LINEAR
			);
			{
				Core::Graphic::Command::ImageMemoryBarrier barrier = Core::Graphic::Command::ImageMemoryBarrier(
					this,
					"Transfer_Level" + std::to_string(generateLevelIndex - 1),
					VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
					VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
					VK_ACCESS_TRANSFER_READ_BIT,
					0
				);
				transferCommandBuffer->AddPipelineImageBarrier(
					VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
					{ &barrier }
				);
			}
		}
		{
			Core::Graphic::Command::ImageMemoryBarrier barrier = Core::Graphic::Command::ImageMemoryBarrier(
				this,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
				VK_ACCESS_TRANSFER_WRITE_BIT,
				0
			);
			transferCommandBuffer->AddPipelineImageBarrier(
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				{ &barrier }
			);
		}

		transferCommandBuffer->EndRecord();
		transferCommandBuffer->Submit();
		transferCommandBuffer->WaitForFinish();
	}

	///Destroy
	{
		for (int levelIndex = 0; levelIndex < needStagingLevelCount; levelIndex++)
		{
			delete perLevelStagingBufffers[levelIndex];
		}
		RemoveImageView("Transfer_AllLevel");
		for (int levelIndex = 0; levelIndex < _mipmapLevelCount; levelIndex++)
		{
			RemoveImageView("Transfer_Level" + std::to_string(levelIndex));
		}
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
		imageInfo.mipLevels = _mipmapLevelCount;
		imageInfo.arrayLayers = _layerCount;
		imageInfo.format = _imageImfo.format;
		imageInfo.tiling = _imageImfo.imageTiling;
		imageInfo.initialLayout = VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = _imageImfo.imageUsageFlags | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
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
			imageView.vkImageSubresourceRange.baseMipLevel = imageInfoPair.second.baseMipmapLevel;
			imageView.vkImageSubresourceRange.levelCount = imageInfoPair.second.mipmapLevelCount;
			imageView.vkImageSubresourceRange.baseArrayLayer = imageInfoPair.second.baseLayer;
			imageView.vkImageSubresourceRange.layerCount = imageInfoPair.second.layerCount;
			imageView.vkExtent2Ds = &_vkExtent2Ds;

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

VkImageView AirEngine::Core::Graphic::Instance::Image::ImageView::VkImageView_()
{
	return vkImageView;
}

const VkImageSubresourceRange& AirEngine::Core::Graphic::Instance::Image::ImageView::VkImageSubresourceRange_()
{
	return vkImageSubresourceRange;
}

uint32_t AirEngine::Core::Graphic::Instance::Image::ImageView::BaseMipmapLevel()
{
	return vkImageSubresourceRange.baseMipLevel;
}

uint32_t AirEngine::Core::Graphic::Instance::Image::ImageView::MipmapLevelCount()
{
	return vkImageSubresourceRange.levelCount;
}

uint32_t AirEngine::Core::Graphic::Instance::Image::ImageView::BaseLayer()
{
	return vkImageSubresourceRange.baseArrayLayer;
}

uint32_t AirEngine::Core::Graphic::Instance::Image::ImageView::LayerCount()
{
	return vkImageSubresourceRange.layerCount;
}

VkImageAspectFlags AirEngine::Core::Graphic::Instance::Image::ImageView::VkImageAspectFlags_()
{
	return vkImageSubresourceRange.aspectMask;
}

VkExtent2D AirEngine::Core::Graphic::Instance::Image::ImageView::VkExtent2D_(uint32_t levelIndex)
{
	return (*vkExtent2Ds)[levelIndex];
}

VkExtent3D AirEngine::Core::Graphic::Instance::Image::ImageView::VkExtent3D_(uint32_t levelIndex)
{
	return { (*vkExtent2Ds)[levelIndex].width, (*vkExtent2Ds)[levelIndex] .height, 1};
}
