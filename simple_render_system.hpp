#pragma once

#include "skoe_pipeline.hpp" 
#include "skoe_device.hpp"
#include "skoe_game_object.hpp"

#include <memory>
#include <vector>

namespace skoe
{
    class SimpleRenderSystem
    {
        public:

            SimpleRenderSystem(SkoeDevice &device, VkRenderPass renderPass);
            ~SimpleRenderSystem();

            SimpleRenderSystem(const SimpleRenderSystem &) = delete;
            SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

            void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<SkoeGameObject> &gameObjects);
            
        private:
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);

            SkoeDevice &skoeDevice;
            std::unique_ptr<SkoePipeline> skoePipeline;
            VkPipelineLayout pipelineLayout;
    };
}