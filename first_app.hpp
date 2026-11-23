#pragma once

#include "skoe_window.hpp"
#include "skoe_device.hpp"
#include "skoe_renderer.hpp"
#include "skoe_game_object.hpp"

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
            void loadGameObjects();

            SkoeWindow skoeWindow{WIDTH, HEIGHT, "Good morning World"};
            SkoeDevice skoeDevice{skoeWindow};
            SkoeRenderer skoeRenderer{skoeWindow, skoeDevice};
            
            std::vector<SkoeGameObject> gameObjects;
    };
}