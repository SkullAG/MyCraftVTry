#pragma once

#include "skoe_window.hpp"
#include "skoe_pipeline.hpp" 
#include "skoe_device.hpp"
#include "skoe_swap_chain.hpp"

#include <memory>
#include <vector>

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
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void drawFrame();

            SkoeWindow skoeWindow{WIDTH, HEIGHT, "Hello world"};
            SkoeDevice skoeDevice{skoeWindow};
            SkoeSwapChain skoeSwapChain{skoeDevice, skoeWindow.getExtent()};
            std::unique_ptr<SkoePipeline> skoePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
    };
}