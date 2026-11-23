#include "first_app.hpp"

// Libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <stdexcept>

namespace skoe {

    struct SimplePushConstantData
    {
        glm::mat2 transform{1.0f};
        glm::vec2 offset;
        // Se usa alignas para declarar en que posicion del buffer esta es decir
        // 8 {x,y,z,4} 16 {r,g,b,a}
        alignas(16) glm::vec3 color;
    };

    FirstApp::FirstApp()
    {
        loadGameObjects();
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

    void FirstApp::loadGameObjects()
    {
        std::vector<SkoeModel::Vertex> vertices 
        {
            {{0.0f, -0.5}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5}, {0.0f, 0.0f, 1.0f}}
        };

        auto skoeModel = std::make_shared<SkoeModel>(skoeDevice, vertices);

        auto triangle = SkoeGameObject::createGameObject();
        triangle.model = skoeModel;
        triangle.color = {0.1f, 0.8f, 0.1f};
        triangle.transform2d.translation.x = 0.2f;
        triangle.transform2d.scale ={2.0f, 0.5f};
        // Funciona en radianes, no grados
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }

    void FirstApp::createPipelineLayout()
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
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
            clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
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

            renderGameObjects(commandBuffers[imageIndex]);

            vkCmdEndRenderPass(commandBuffers[imageIndex]);
            if(vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {throw std::runtime_error("Failed to record command buffer ");}

    }

    void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer)
    {

        int i = 0;

        for (auto& obj : gameObjects)
        {
            i += 1;
            // Ponle rotoscopia
            obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.0001f, glm::two_pi<float>());
        }
        skoePipeline->bind(commandBuffer);

        for (auto& obj : gameObjects)
        {
            

            SimplePushConstantData push{};
            push.offset = obj.transform2d.translation;
            push.color = obj.color;
            push.transform = obj.transform2d.mat2();

            vkCmdPushConstants(
                commandBuffer, 
                pipelineLayout, 
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 
                0, 
                sizeof(SimplePushConstantData), 
                &push);
            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
        
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