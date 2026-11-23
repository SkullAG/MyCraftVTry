#pragma once

#include "skoe_window.hpp"
#include "skoe_pipeline.hpp" 
#include "skoe_device.hpp"
#include "skoe_swap_chain.hpp"
#include "skoe_game_object.hpp"

#include <memory>
#include <vector>
#include <glm/glm/glm.hpp>

namespace skoe
{
    class FirstApp
    {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            FirstApp();
            ~FirstApp();

            FirstApp(const FirstApp &) = delete;
            FirstApp &operator=(const FirstApp &) = delete;

            void run();
            
        private:
            void loadGameObjects();
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void freeCommandBuffers();
            void drawFrame();
            void recreateSwapChain();
            void recordCommandBuffer(int imageIndex);
            void renderGameObjects(VkCommandBuffer commandBuffer);

            SkoeWindow skoeWindow{WIDTH, HEIGHT, "Hello world"};
            SkoeDevice skoeDevice{skoeWindow};
            std::unique_ptr<SkoeSwapChain>skoeSwapChain;
            std::unique_ptr<SkoePipeline> skoePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::vector<SkoeGameObject> gameObjects;
    };
}