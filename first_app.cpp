#include "first_app.hpp"

#include "simple_render_system.hpp"

// Libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <stdexcept>

namespace skoe {

    FirstApp::FirstApp()
    {
        loadGameObjects();
    }

    FirstApp::~FirstApp(){}

    void FirstApp::run()
    {
        SimpleRenderSystem simpleRenderSystem{skoeDevice, skoeRenderer.getSwapChainRenderPass()};
        while(!skoeWindow.shouldClose())
        {
            glfwPollEvents();
            
            if(auto commandBuffer = skoeRenderer.beginFrame())
            {
                // Begin offscreen shadow pass
                // render shadow casting objects
                // end offscren shadow pass

                skoeRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                skoeRenderer.endSwapChainRenderPass(commandBuffer);
                skoeRenderer.endFrame();
            }
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

}