#include "first_app.hpp"

#include <array>
#include <cassert>
#include <stdexcept>

namespace skoe {

    FirstApp::FirstApp()
    {
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
        createCommandBuffers();
    }

    FirstApp::~FirstApp()
    {
        vkDestroyPipelineLayout(skoeDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run()
    {
        while(!skoeWindow.shouldClose())
        {
            glfwPollEvents();
            drawFrame();
        }

        vkDeviceWaitIdle(skoeDevice.device());
    }

    void FirstApp::loadModels()
    {
        std::vector<SkoeModel::Vertex> vertices 
        {
            {{0.0f, -0.5}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5}, {0.0f, 0.0f, 1.0f}}
        };

        skoeModel = std::make_unique<SkoeModel>(skoeDevice, vertices);
    }

    void FirstApp::createPipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if(vkCreatePipelineLayout(skoeDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout");
        }
        
    }

    void FirstApp::recreateSwapChain()
    {
        auto extent = skoeWindow.getExtent();
        while(extent.width == 0 || extent.height == 0)
        {
            extent = skoeWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(skoeDevice.device());

        //skoeSwapChain = nullptr;

        if(skoeSwapChain == nullptr)
        {
            skoeSwapChain = std::make_unique<SkoeSwapChain>(skoeDevice, extent);
        } else
        {
            skoeSwapChain = std::make_unique<SkoeSwapChain>(skoeDevice, extent, std::move(skoeSwapChain));
            if(skoeSwapChain->imageCount() != commandBuffers.size())
            {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }
        
        createPipeline();
    }

    void FirstApp::createPipeline()
    {
        assert(skoeSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig = {};
        SkoePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = skoeSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        skoePipeline = std::make_unique<SkoePipeline>(
            skoeDevice,
            "assets/shaders/shader.vert.spv",
            "assets/shaders/shader.frag.spv",
            pipelineConfig);

    }

    void FirstApp::createCommandBuffers()
    {
        commandBuffers.resize(skoeSwapChain->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = skoeDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if(vkAllocateCommandBuffers(skoeDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate comand buffer");
        }

    }

    void FirstApp::freeCommandBuffers()
    {
        vkFreeCommandBuffers(
            skoeDevice.device(), 
            skoeDevice.getCommandPool(), 
            static_cast<uint32_t>(commandBuffers.size()), 
            commandBuffers.data());
        commandBuffers.clear();
    }

    void FirstApp::recordCommandBuffer(int imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if(vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {throw std::runtime_error("Failed to begin recording command buffer");}

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = skoeSwapChain->getRenderPass();
            renderPassInfo.framebuffer = skoeSwapChain->getFrameBuffer(imageIndex);

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = skoeSwapChain->getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport{};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(skoeSwapChain->getSwapChainExtent().width);
            viewport.height =  static_cast<float>(skoeSwapChain->getSwapChainExtent().height);
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            VkRect2D scissor{{0, 0}, skoeSwapChain->getSwapChainExtent()};
            vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
            vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

            skoePipeline->bind(commandBuffers[imageIndex]);
            skoeModel->bind(commandBuffers[imageIndex]);
            skoeModel->draw(commandBuffers[imageIndex]);

            vkCmdEndRenderPass(commandBuffers[imageIndex]);
            if(vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {throw std::runtime_error("Failed to record command buffer ");}

    }
    
    void  FirstApp::drawFrame()
    {
        uint32_t imageIndex;
        auto result = skoeSwapChain->acquireNextImage(&imageIndex);

        if(result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return;
        }
        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire swap chain image ");
        }

        recordCommandBuffer(imageIndex);
        result = skoeSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || skoeWindow.wasWindowResized())
        {
            skoeWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }

        if(result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swap chain image ");
        }

    }

}