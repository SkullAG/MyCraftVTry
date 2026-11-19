#pragma once

#include "skoe_device.hpp"

// std
#include <string>
#include <vector>

namespace skoe {

    struct PipelineConfigInfo 
    {
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class SkoePipeline{
        public:
            SkoePipeline(
                SkoeDevice& device, 
                const std::string& vertFilePath,
                const std::string& fragFilePath, 
                const PipelineConfigInfo& configInfo);
            ~SkoePipeline();

            SkoePipeline(const SkoePipeline&) = delete;
            void operator=(const SkoePipeline&) = delete;

            void bind(VkCommandBuffer commandBuffer);
            static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

        private:
            static std::vector<char> readFile(const std::string& filePath);
            void createGraphicsPipeline(
                const std::string& vertFilePath,
                const std::string& fragFilePath, 
                const PipelineConfigInfo& configInfo);

            void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

            SkoeDevice& skoeDevice;
            VkPipeline graphicsPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
}