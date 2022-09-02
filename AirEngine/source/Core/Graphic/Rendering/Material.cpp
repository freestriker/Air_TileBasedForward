#include "Core/Graphic/Rendering/Material.h"
#include "Asset/Mesh.h"
#include "Asset/Texture2D.h"
#include "Asset/TextureCube.h"
#include "Core/Graphic/Instance/Buffer.h"
#include "Core/Graphic/Instance/DescriptorSet.h"
#include "Core/Graphic/Instance/Image.h"
#include "Core/Graphic/Instance/ImageSampler.h"
#include "Core/Graphic/Rendering/Shader.h"
#include "Core/Graphic/Manager/DescriptorSetManager.h"
#include "Core/Graphic/CoreObject/Instance.h"
#include "Utils/Log.h"

AirEngine::Asset::TextureCube* AirEngine::Core::Graphic::Rendering::Material::GetTextureCube(std::string name)
{
	if (_slots.count(name) && (_slots[name].slotType == ShaderSlotType::TEXTURE_CUBE))
	{
		return static_cast<Asset::TextureCube*>(_slots[name].asset);
	}
	else
	{
		Utils::Log::Exception("Failed to get textureCube.");
		return nullptr;
	}
}

void AirEngine::Core::Graphic::Rendering::Material::SetTextureCube(std::string name, Asset::TextureCube* textureCube)
{
	if (_slots.count(name) && _slots[name].slotType == ShaderSlotType::TEXTURE_CUBE)
	{
		_slots[name].asset = textureCube;
		_slots[name].descriptorSet->UpdateBindingData(
			{ 0 },
			{
				{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, textureCube->ImageSampler()->VkSampler_(), textureCube->Image()->VkImageView_(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL}
			}
		);
	}
	else
	{
		Utils::Log::Exception("Failed to set textureCube.");
	}
}

AirEngine::Asset::Texture2D* AirEngine::Core::Graphic::Rendering::Material::GetTexture2D(std::string name)
{
	if (_slots.count(name) && (_slots[name].slotType == ShaderSlotType::TEXTURE2D || _slots[name].slotType == ShaderSlotType::TEXTURE2D_WITH_INFO))
	{
		return static_cast<Asset::Texture2D*>(_slots[name].asset);
	}
	else
	{
		Utils::Log::Exception("Failed to get texture2d.");
		return nullptr;
	}
}

void AirEngine::Core::Graphic::Rendering::Material::SetTexture2D(std::string name, Asset::Texture2D* texture2d)
{
	if (_slots.count(name) && _slots[name].slotType == ShaderSlotType::TEXTURE2D)
	{
		_slots[name].asset = texture2d;
		_slots[name].descriptorSet->UpdateBindingData(
			{ 0 },
			{ {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, texture2d->ImageSampler()->VkSampler_(), texture2d->Image()->VkImageView_(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL} }
		);
	}
	else if (_slots.count(name) && _slots[name].slotType == ShaderSlotType::TEXTURE2D_WITH_INFO)
	{
		_slots[name].asset = texture2d;
		_slots[name].descriptorSet->UpdateBindingData(
			{ 0, 1 },
			{
				{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, texture2d->ImageSampler()->VkSampler_(), texture2d->Image()->VkImageView_(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL},
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, texture2d->InfoBuffer()->VkBuffer_(), texture2d->InfoBuffer()->Offset(), texture2d->InfoBuffer()->Size()}
			}
			);
	}
	else
	{
		Utils::Log::Exception("Failed to set texture2d.");
	}
}

AirEngine::Asset::Texture2D* AirEngine::Core::Graphic::Rendering::Material::GetStorageTexture2D(std::string name)
{
	if (_slots.count(name) && (_slots[name].slotType == ShaderSlotType::STORAGE_TEXTURE2D || _slots[name].slotType == ShaderSlotType::STORAGE_TEXTURE2D_WITH_INFO))
	{
		return static_cast<Asset::Texture2D*>(_slots[name].asset);
	}
	else
	{
		Utils::Log::Exception("Failed to get storge texture2d.");
		return nullptr;
	}
}

void AirEngine::Core::Graphic::Rendering::Material::SetStorgeTexture2D(std::string name, Asset::Texture2D* texture2d)
{
	if (_slots.count(name) && _slots[name].slotType == ShaderSlotType::STORAGE_TEXTURE2D)
	{
		_slots[name].asset = texture2d;
		_slots[name].descriptorSet->UpdateBindingData(
			{ 0 },
			{ {VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, texture2d->ImageSampler()->VkSampler_(), texture2d->Image()->VkImageView_(), VK_IMAGE_LAYOUT_GENERAL} }
		);
	}
	else if (_slots.count(name) && _slots[name].slotType == ShaderSlotType::STORAGE_TEXTURE2D_WITH_INFO)
	{
		_slots[name].asset = texture2d;
		_slots[name].descriptorSet->UpdateBindingData(
			{ 0, 1 },
			{
				{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, texture2d->ImageSampler()->VkSampler_(), texture2d->Image()->VkImageView_(), VK_IMAGE_LAYOUT_GENERAL},
				{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, texture2d->InfoBuffer()->VkBuffer_(), texture2d->InfoBuffer()->Offset(), texture2d->InfoBuffer()->Size()}
			}
			);
	}
	else
	{
		Utils::Log::Exception("Failed to set storage texture2d.");
	}
}

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

void AirEngine::Core::Graphic::Rendering::Material::RefreshSlotData(std::vector<std::string> slotNames)
{
	for (const auto& slotName : slotNames)
	{
		const auto& slot = _slots[slotName];
		switch (slot.slotType)
		{
			case ShaderSlotType::UNIFORM_BUFFER:
			{
				Instance::Buffer* ub = static_cast<Instance::Buffer*>(slot.asset);
				slot.descriptorSet->UpdateBindingData({ 0 }, { Graphic::Instance::DescriptorSet::DescriptorSetWriteData(VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, ub->VkBuffer_(), ub->Offset(), ub->Size()) });
				break;
			}
			case ShaderSlotType::STORAGE_BUFFER:
			{
				Instance::Buffer* ub = static_cast<Instance::Buffer*>(slot.asset);
				slot.descriptorSet->UpdateBindingData({ 0 }, { Graphic::Instance::DescriptorSet::DescriptorSetWriteData(VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, ub->VkBuffer_(), ub->Offset(), ub->Size()) });
				break;
			}
			case ShaderSlotType::UNIFORM_TEXEL_BUFFER:
			{
				Instance::Buffer* ub = static_cast<Instance::Buffer*>(slot.asset);
				slot.descriptorSet->UpdateBindingData({ 0 }, { Graphic::Instance::DescriptorSet::DescriptorSetWriteData(VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, ub->VkBuffer_(), ub->VkBufferView_(), ub->Offset(), ub->Size()) });
				break;
			}
			case ShaderSlotType::STORAGE_TEXEL_BUFFER:
			{
				Instance::Buffer* ub = static_cast<Instance::Buffer*>(slot.asset);
				slot.descriptorSet->UpdateBindingData({ 0 }, { Graphic::Instance::DescriptorSet::DescriptorSetWriteData(VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, ub->VkBuffer_(), ub->VkBufferView_(), ub->Offset(), ub->Size()) });
				break;
			}

			case ShaderSlotType::TEXTURE2D:
			{
				Asset::Texture2D* t = static_cast<Asset::Texture2D*>(slot.asset);
				slot.descriptorSet->UpdateBindingData({ 0 }, { Graphic::Instance::DescriptorSet::DescriptorSetWriteData(VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, t->ImageSampler()->VkSampler_(), t->Image()->VkImageView_(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) });
				break;
			}
			case ShaderSlotType::TEXTURE2D_WITH_INFO:
			{
				Asset::Texture2D* t = static_cast<Asset::Texture2D*>(slot.asset);
				slot.descriptorSet->UpdateBindingData({ 0, 1 }, {
					Graphic::Instance::DescriptorSet::DescriptorSetWriteData(VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, t->ImageSampler()->VkSampler_(), t->Image()->VkImageView_(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL),
					Graphic::Instance::DescriptorSet::DescriptorSetWriteData(VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, t->InfoBuffer()->VkBuffer_(), t->InfoBuffer()->Offset(), t->InfoBuffer()->Size())
					});

				break;
			}
			case ShaderSlotType::STORAGE_TEXTURE2D:
			{
				Asset::Texture2D* t = static_cast<Asset::Texture2D*>(slot.asset);
				slot.descriptorSet->UpdateBindingData({ 0 }, { Graphic::Instance::DescriptorSet::DescriptorSetWriteData(VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_NULL_HANDLE, t->Image()->VkImageView_(), VK_IMAGE_LAYOUT_GENERAL) });
				break;
			}
			case ShaderSlotType::STORAGE_TEXTURE2D_WITH_INFO:
			{
				Asset::Texture2D* t = static_cast<Asset::Texture2D*>(slot.asset);
				slot.descriptorSet->UpdateBindingData({ 0, 1 }, {
					Graphic::Instance::DescriptorSet::DescriptorSetWriteData(VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, t->ImageSampler()->VkSampler_(), t->Image()->VkImageView_(), VK_IMAGE_LAYOUT_GENERAL),
					Graphic::Instance::DescriptorSet::DescriptorSetWriteData(VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, t->InfoBuffer()->VkBuffer_(), t->InfoBuffer()->Offset(), t->InfoBuffer()->Size())
					});

				break;
			}

		}
	}
}

void AirEngine::Core::Graphic::Rendering::Material::SetSlotData(std::string name, std::vector<uint32_t> bindingIndex, std::vector<Graphic::Instance::DescriptorSet::DescriptorSetWriteData> data)
{
	_slots[name].asset = nullptr;
	_slots[name].descriptorSet->UpdateBindingData(bindingIndex, data);
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
