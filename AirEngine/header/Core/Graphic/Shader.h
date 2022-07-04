#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <memory>
#include <string>
#include <json.hpp>
#include <map>
#include "Utils/SpirvReflect.h"
#include "Core/IO/Asset/AssetBase.h"
#include "glm/glm.hpp"
namespace AirEngine
{
	namespace Core
	{
		namespace Graphic
		{
			enum class ShaderSlotType
			{
				UNIFORM_BUFFER,
				STORAGE_BUFFER,
				UNIFORM_TEXEL_BUFFER,
				STORAGE_TEXEL_BUFFER,
				TEXTURE2D,
				TEXTURE2D_WITH_INFO,
				STORAGE_TEXTURE2D,
				STORAGE_TEXTURE2D_WITH_INFO,
				TEXTURE_CUBE
			};
			class Shader final: public IO::Asset::AssetBase
			{
			public:
				enum class ShaderType
				{
					GRAPHIC,
					COMPUTE,
				};
				struct SlotDescriptor
				{
					std::string name{};
					uint32_t setIndex{};
					ShaderSlotType slotType{};
					VkDescriptorSetLayout vkDescriptorSetLayout{};
					std::vector<VkDescriptorType> vkDescriptorTypes{};
				};
				struct ShaderSettings
				{
					std::string renderPass{};
					std::string subpass{};
					std::vector<std::string> shaderPaths{};
					VkCullModeFlags cullMode{};
					VkBool32 blendEnable{};
					VkBlendFactor srcColorBlendFactor{};
					VkBlendFactor dstColorBlendFactor{};
					VkBlendOp colorBlendOp{};
					VkBlendFactor srcAlphaBlendFactor{};
					VkBlendFactor dstAlphaBlendFactor{};
					VkBlendOp alphaBlendOp{};
					VkColorComponentFlags colorWriteMask{};
					VkBool32 depthTestEnable{};
					VkBool32 depthWriteEnable{};
					VkCompareOp depthCompareOp{};

					NLOHMANN_DEFINE_TYPE_INTRUSIVE
					(
						ShaderSettings, 
						shaderPaths, 
						cullMode, 
						blendEnable, 
						srcColorBlendFactor, 
						dstColorBlendFactor, 
						colorBlendOp, 
						srcAlphaBlendFactor, 
						dstAlphaBlendFactor, 
						alphaBlendOp, 
						colorWriteMask, 
						renderPass, 
						subpass, 
						depthTestEnable,
						depthWriteEnable, 
						depthCompareOp
					);
				};
			private:
				struct VertexData
				{
					glm::vec3 position{};
					glm::vec2 texCoords{};
					glm::vec3 normal{};
					glm::vec3 tangent{};
					glm::vec3 bitangent{};
				};

				struct _ShaderModuleWrapper
				{
					VkShaderStageFlagBits stage{};
					VkShaderModule shaderModule{};
					SpvReflectShaderModule reflectModule{};
				};
				struct _PipelineData
				{
					std::map<std::string, std::vector<char>> spirvs{};
					std::vector<_ShaderModuleWrapper> shaderModuleWrappers{};
					std::vector< VkPipelineShaderStageCreateInfo> stageInfos{};

					VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
					VkVertexInputBindingDescription vertexInputBindingDescription{};
					std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions{};

					VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
					VkViewport viewport{};
					VkRect2D scissor{};
					VkPipelineViewportStateCreateInfo viewportState{};
					VkPipelineRasterizationStateCreateInfo rasterizer{};
					VkPipelineMultisampleStateCreateInfo multisampling{};
					VkPipelineDepthStencilStateCreateInfo depthStencil{};
					VkPipelineColorBlendAttachmentState colorBlendAttachment{};
					VkPipelineColorBlendStateCreateInfo colorBlending{};

					std::vector<VkDescriptorSetLayout> descriptorSetLayouts{};

				};
			private:
				ShaderSettings _shaderSettings;
				std::map<std::string, SlotDescriptor> _slotDescriptors;
				VkPipeline _vkPipeline;
				VkPipelineLayout _vkPipelineLayout;
				ShaderType _shaderType;

				void _ParseShaderData(_PipelineData& pipelineData);
				void _LoadSpirvs(_PipelineData& pipelineData);
				void _CreateShaderModules(_PipelineData& pipelineData);
				void _PopulateShaderStages(_PipelineData& pipelineData);
				void _PopulateVertexInputState(_PipelineData& pipelineData);
				void _CheckAttachmentOutputState(_PipelineData& pipelineData);
				void _PopulateGraphicPipelineSettings(_PipelineData& pipelineData);
				void _CreateDescriptorLayouts(_PipelineData& pipelineData);
				void _PopulateDescriptorLayouts(_PipelineData& pipelineData);
				void _CreateGraphicPipeline(_PipelineData& pipelineData);
				void _CreateComputePipeline(_PipelineData& pipelineData);
				void _DestroyData(_PipelineData& pipelineData);
				void OnLoad(Core::Graphic::Command::CommandBuffer* transferCommandBuffer);
			public:

				const std::map<std::string, SlotDescriptor>* SlotDescriptors();
				VkPipeline VkPipeline_();
				VkPipelineLayout VkPipelineLayout_();
				const ShaderSettings* Settings();
				ShaderType ShaderType_();
				Shader();
				~Shader();
				Shader(const Shader&) = delete;
				Shader& operator=(const Shader&) = delete;
				Shader(Shader&&) = delete;
				Shader& operator=(Shader&&) = delete;
			};

		}
	}
}