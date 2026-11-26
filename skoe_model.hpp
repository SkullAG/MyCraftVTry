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
                glm::vec3 position;
                glm::vec3 color;
                
                static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            };

            struct Builder 
            {
                std::vector<Vertex> vertices{};
                std::vector<uint32_t> indices{};
            };

            SkoeModel(SkoeDevice &device, const SkoeModel::Builder &builder);
            ~SkoeModel();

            SkoeModel(const SkoeModel &) = delete;
            SkoeModel &operator=(const SkoeModel &) = delete;

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);

        private:
            void createVertexBuffers(const std::vector<Vertex> &vertices);
            void createIndexBuffers(const std::vector<uint32_t> &indices);

            SkoeDevice &skoeDevice;

            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            uint32_t vertexCount;

            bool hasIndexBuffer = false;
            VkBuffer indexBuffer;
            VkDeviceMemory indexBufferMemory;
            uint32_t indexCount;

    };


}