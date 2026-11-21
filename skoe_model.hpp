#pragma once

#include "skoe_device.hpp"

// Libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm/glm.hpp>

// std
#include <vector>

namespace skoe
{
    class SkoeModel
    {
        public:

            struct Vertex
            {
                glm::vec2 position;
                glm::vec3 color;
                
                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            };

            SkoeModel(SkoeDevice &device, const std::vector<Vertex> &vertices);
            ~SkoeModel();

            SkoeModel(const SkoeModel &) = delete;
            SkoeModel &operator=(const SkoeModel &) = delete;

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);

        private:
            void createVertexBuffers(const std::vector<Vertex> &vertices);

            SkoeDevice &skoeDevice;
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            uint32_t vertexCount;

    };


}