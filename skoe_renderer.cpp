#include "skoe_renderer.hpp"

#include <array>
#include <cassert>
#include <stdexcept>

namespace skoe {

    SkoeRenderer::SkoeRenderer(SkoeWindow &window, SkoeDevice &device) : skoeWindow{window}, skoeDevice{device}
    {
        recreateSwapChain();
        createCommandBuffers();
    }

    SkoeRenderer::~SkoeRenderer()
    {
        freeCommandBuffers();
    }

    void SkoeRenderer::recreateSwapChain()
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
            std::shared_ptr<SkoeSwapChain> oldSwapChain = std::move(skoeSwapChain);
            skoeSwapChain = std::make_unique<SkoeSwapChain>(skoeDevice, extent, oldSwapChain);
            
            if(!oldSwapChain->compareSwapFormats(*skoeSwapChain.get()))
            {
                throw std::runtime_error("Swap chain image(or depth) format has changed!");
            }
        }
        
        // well be back
    }

    void SkoeRenderer::createCommandBuffers()
    {
        commandBuffers.resize(SkoeSwapChain::MAX_FRAMES_IN_FLIGHT);

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

    void SkoeRenderer::freeCommandBuffers()
    {
        vkFreeCommandBuffers(
            skoeDevice.device(), 
            skoeDevice.getCommandPool(), 
            static_cast<uint32_t>(commandBuffers.size()), 
            commandBuffers.data());
        commandBuffers.clear();
    }

    VkCommandBuffer SkoeRenderer::beginFrame()
    {
        assert(!isFrameStarted && "Can't call beginFrame while already in progress");

        auto result = skoeSwapChain->acquireNextImage(&currentImageIndex);

        if(result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain();
            return nullptr;
        }
        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("Failed to acquire swap chain image ");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {throw std::runtime_error("Failed to begin recording command buffer");}

        return commandBuffer;
    }

    void SkoeRenderer::endFrame()
    {
        assert(isFrameStarted && "Can't call endFrame while frame is not in progress");

        auto commandBuffer = getCurrentCommandBuffer();
        if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {throw std::runtime_error("Failed to record command buffer ");}

        auto result = skoeSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || skoeWindow.wasWindowResized())
        {
            skoeWindow.resetWindowResizedFlag();
            recreateSwapChain();
        }else if(result != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to present swap chain image ");
        }

        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1) % SkoeSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void SkoeRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Can't call beginSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't call begin render pass on command buffer from a different frame");
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = skoeSwapChain->getRenderPass();
        renderPassInfo.framebuffer = skoeSwapChain->getFrameBuffer(currentImageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = skoeSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(skoeSwapChain->getSwapChainExtent().width);
        viewport.height =  static_cast<float>(skoeSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, skoeSwapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
    }

    void SkoeRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer)
    {
        assert(isFrameStarted && "Can't call endSwapChainRenderPass if frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Can't call end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);
    }

}