#include "first_app.hpp"

namespace skoe {
    void FirstApp::run(){
        while(!skoeWindow.shouldClose())
        {
            glfwPollEvents();
        }
    }
}