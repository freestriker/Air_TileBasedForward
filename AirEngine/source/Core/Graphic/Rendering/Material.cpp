#include "Core/Graphic/Rendering/Material.h"
#include "Asset/Mesh.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/DescriptorSet.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Core/Graphic/Manager/DescriptorSetManager.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Utils/Log.h"

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::Rendering::Material::GetUniformBuffer(std::string name)
{
	if (_slots.count(name) && _slots[name].slotType == ShaderSlotType::UNIFORM_BUFFER)
	{
		return static_cast<Graphic::Instance::Buffer*>(_slots[name].asset);
	}
	else
	{
		Utils::Log::Exception("Failed to get uniform buffer.");
		return nullptr;
	}
}

void AirEngine::Core::Graphic::Rendering::Material::SetUniformBuffer(std::string name, Instance::Buffer* buffer)
{
	if (_slots.count(name) && _slots[name].slotType == ShaderSlotType::UNIFORM_BUFFER)
	{
		_slots[name].asset = buffer;
		_slots[name].descriptorSet->UpdateBindingData(
			{ 0 },
			{
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, buffer->VkBuffer_(), buffer->Offset(), buffer->Size()}
			}
			);
	}
	else
	{
		Utils::Log::Exception("Failed to set uniform buffer.");
	}
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::Rendering::Material::GetStorageBuffer(std::string name)
{
	if (_slots.count(name) && _slots[name].slotType == ShaderSlotType::STORAGE_BUFFER)
	{
		return static_cast<Graphic::Instance::Buffer*>(_slots[name].asset);
	}
	else
	{
		Utils::Log::Exception("Failed to get storage buffer.");
		return nullptr;
	}
}

void AirEngine::Core::Graphic::Rendering::Material::SetStorageBuffer(std::string name, Instance::Buffer* buffer)
{
	if (_slots.count(name) && _slots[name].slotType == ShaderSlotType::STORAGE_BUFFER)
	{
		_slots[name].asset = buffer;
		_slots[name].descriptorSet->UpdateBindingData(
			{ 0 },
			{
				{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, buffer->VkBuffer_(), buffer->Offset(), buffer->Size()}
			}
			);
	}
	else
	{
		Utils::Log::Exception("Failed to set storage buffer.");
	}
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::Rendering::Material::GetUniformTexelBuffer(std::string name)
{
	if (_slots.count(name) && _slots[name].slotType == ShaderSlotType::UNIFORM_TEXEL_BUFFER)
	{
		return static_cast<Graphic::Instance::Buffer*>(_slots[name].asset);
	}
	else
	{
		Utils::Log::Exception("Failed to get uniform texel buffer.");
		return nullptr;
	}
}

void AirEngine::Core::Graphic::Rendering::Material::SetUniformTexelBuffer(std::string name, Instance::Buffer* buffer)
{
	if (_slots.count(name) && _slots[name].slotType == ShaderSlotType::UNIFORM_TEXEL_BUFFER)
	{
		_slots[name].asset = buffer;
		_slots[name].descriptorSet->UpdateBindingData(
			{ 0 },
			{
				{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, buffer->VkBuffer_(), buffer->VkBufferView_(), buffer->Offset(), buffer->Size()}
			}
			);
	}
	else
	{
		Utils::Log::Exception("Failed to set uniform texel buffer.");
	}
}

AirEngine::Core::Graphic::Instance::Buffer* AirEngine::Core::Graphic::Rendering::Material::GetStorgeTexelBuffer(std::string name)
{
	if (_slots.count(name) && _slots[name].slotType == ShaderSlotType::STORAGE_TEXEL_BUFFER)
	{
		return static_cast<Graphic::Instance::Buffer*>(_slots[name].asset);
	}
	else
	{
		Utils::Log::Exception("Failed to get storage texel buffer.");
		return nullptr;
	}
}

void AirEngine::Core::Graphic::Rendering::Material::SetStorgeTexelBuffer(std::string name, Instance::Buffer* buffer)
{
	if (_slots.count(name) && _slots[name].slotType == ShaderSlotType::STORAGE_TEXEL_BUFFER)
	{
		_slots[name].asset = buffer;
		_slots[name].descriptorSet->UpdateBindingData(
			{ 0 },
			{
				{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, buffer->VkBuffer_(), buffer->VkBufferView_(), buffer->Offset(), buffer->Size()}
			}
			);
	}
	else
	{
		Utils::Log::Exception("Failed to set storage texel buffer.");
	}
}

VkPipelineLayout AirEngine::Core::Graphic::Rendering::Material::PipelineLayout()
{
	return this->_shader->VkPipelineLayout_();
}

std::vector<VkDescriptorSet> AirEngine::Core::Graphic::Rendering::Material::VkDescriptorSets()
{
	std::vector<VkDescriptorSet> sets = std::vector<VkDescriptorSet>(_slots.size());

	for (const auto& slotPair : _slots)
	{
		sets[slotPair.second.setIndex] = slotPair.second.descriptorSet->VkDescriptorSet_();
	}
	return sets;
}

AirEngine::Core::Graphic::Rendering::Shader* AirEngine::Core::Graphic::Rendering::Material::Shader()
{
	return _shader;
}

void AirEngine::Core::Graphic::Rendering::Material::SetSlotData(std::string name, std::vector<uint32_t> bindingIndex, std::vector<Graphic::Instance::DescriptorSet::DescriptorSetWriteData> data)
{
	_slots[name].asset = nullptr;
	_slots[name].descriptorSet->UpdateBindingData(bindingIndex, data);
}
AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Rendering::Material::GetSampledImageCube(std::string name)
{
	if (_slots.count(name) && (_slots[name].slotType == ShaderSlotType::TEXTURE_CUBE))
	{
		return static_cast<AirEngine::Core::Graphic::Instance::Image*>(_slots[name].asset);
	}
	else
	{
		Utils::Log::Exception("Failed to get textureCube.");
		return nullptr;
	}
}
void AirEngine::Core::Graphic::Rendering::Material::SetSampledImageCube(std::string slotName, AirEngine::Core::Graphic::Instance::Image* imageCube, AirEngine::Core::Graphic::Instance::ImageSampler* sampler, std::string imageViewName)
{
	if (_slots.count(slotName) && _slots[slotName].slotType == ShaderSlotType::TEXTURE_CUBE)
	{
		_slots[slotName].asset = imageCube;
		_slots[slotName].descriptorSet->UpdateBindingData(
			{ 0 },
			{
				{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, sampler->VkSampler_(), imageCube->ImageView_(imageViewName).vkImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
			}
			);
	}
	else
	{
		Utils::Log::Exception("Failed to set textureCube.");
	}
}
AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Rendering::Material::GetSampledImage2D(std::string name)
{
	if (_slots.count(name) && (_slots[name].slotType == ShaderSlotType::TEXTURE2D))
	{
		return static_cast<AirEngine::Core::Graphic::Instance::Image*>(_slots[name].asset);
	}
	else
	{
		Utils::Log::Exception("Failed to get texture2D.");
		return nullptr;
	}
}
void AirEngine::Core::Graphic::Rendering::Material::SetSampledImage2D(std::string slotName, AirEngine::Core::Graphic::Instance::Image* image2D, AirEngine::Core::Graphic::Instance::ImageSampler* sampler, std::string imageViewName)
{
	if (_slots.count(slotName) && _slots[slotName].slotType == ShaderSlotType::TEXTURE2D)
	{
		_slots[slotName].asset = image2D;
		_slots[slotName].descriptorSet->UpdateBindingData(
			{ 0 },
			{
				{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, sampler->VkSampler_(), image2D->ImageView_(imageViewName).vkImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
			}
			);
	}
	else
	{
		Utils::Log::Exception("Failed to set texture2D.");
	}
}
AirEngine::Core::Graphic::Instance::Image* AirEngine::Core::Graphic::Rendering::Material::GetStorageImage2D(std::string name)
{
	if (_slots.count(name) && (_slots[name].slotType == ShaderSlotType::STORAGE_TEXTURE2D))
	{
		return static_cast<AirEngine::Core::Graphic::Instance::Image*>(_slots[name].asset);
	}
	else
	{
		Utils::Log::Exception("Failed to get storage texture2D.");
		return nullptr;
	}
}
void AirEngine::Core::Graphic::Rendering::Material::SetStorageImage2D(std::string slotName, AirEngine::Core::Graphic::Instance::Image* image2D, std::string imageViewName)
{
	if (_slots.count(slotName) && _slots[slotName].slotType == ShaderSlotType::STORAGE_TEXTURE2D)
	{
		_slots[slotName].asset = image2D;
		_slots[slotName].descriptorSet->UpdateBindingData(
			{ 0 },
			{
				{VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE, image2D->ImageView_(imageViewName).vkImageView, VkImageLayout::VK_IMAGE_LAYOUT_GENERAL}
			}
			);
	}
	else
	{
		Utils::Log::Exception("Failed to set storage texture2D.");
	}
}
void AirEngine::Core::Graphic::Rendering::Material::OnDestroy()
{
}

AirEngine::Core::Graphic::Rendering::Material::Material(AirEngine::Core::Graphic::Rendering::Shader* shader)
	: _shader(shader)
	, _slots()
{
	for (const auto& pair : *shader->SlotDescriptors())
	{
		Slot newSlot = Slot();
		newSlot.asset = nullptr;
		newSlot.name = pair.second.name;
		newSlot.slotType = pair.second.slotType;
		newSlot.descriptorSet = CoreObject::Instance::DescriptorSetManager().AcquireDescripterSet(pair.second.slotType, pair.second.vkDescriptorSetLayout);
		newSlot.setIndex = pair.second.setIndex;
		_slots.emplace(newSlot.name, newSlot);
	}
}

AirEngine::Core::Graphic::Rendering::Material::~Material()
{

}
