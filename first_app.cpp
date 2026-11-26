#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "skoe_camera.hpp"
#include "simple_render_system.hpp"

// Libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/constants.hpp>

#include <array>
#include <chrono>
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
        SkoeCamera camera{};
        // camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 0.5f));

        auto viewerObject = SkoeGameObject::createGameObject();
        KeyboardMovementController cameraController{};
        
        auto currentTime = std::chrono::high_resolution_clock::now();

        while(!skoeWindow.shouldClose())
        {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime-currentTime).count();
            currentTime = newTime;

            cameraController.moveInPlaneXZ(skoeWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = skoeRenderer.getAspectRatio();

            //camera.setOrthographicProjection(glm::vec2(aspect, 1)*2.0f, -1, 10);
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 10);

            camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.f);

            if(auto commandBuffer = skoeRenderer.beginFrame())
            {
                // Begin offscreen shadow pass
                // render shadow casting objects
                // end offscren shadow pass

                skoeRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
                skoeRenderer.endSwapChainRenderPass(commandBuffer);
                skoeRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(skoeDevice.device());
    }

    std::unique_ptr<SkoeModel> createCubeModel(SkoeDevice& device, glm::vec3 offset) {
        SkoeModel::Builder modelBuilder{};
        modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
        
            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
        
            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        
            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        
            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        
            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };
        for (auto& v : modelBuilder.vertices) {
            v.position += offset;
        }
        
        modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};
        
        return std::make_unique<SkoeModel>(device, modelBuilder);
    }

    void FirstApp::loadGameObjects()
    {
        std::shared_ptr<SkoeModel> skoeModel = createCubeModel(skoeDevice, {0.0f, 0.0f, 0.0f});
        
        auto cube = SkoeGameObject::createGameObject();
        cube.model = skoeModel;
        cube.transform.translation = {0.0f, 0.0f, 0.5f};
        cube.transform.scale = {0.5f, 0.5f , 0.5f};
        gameObjects.push_back(std::move(cube));
    }

}