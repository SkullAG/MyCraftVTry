#pragma once

#include "skoe_window.hpp"

namespace skoe
{
    class FirstApp
    {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            void run();
            
        private:
            SkoeWindow skoeWindow{WIDTH, HEIGHT, "Hello world"};
    };
}