#include "skoe_window.hpp"
#include <stdexcept>
namespace skoe
{

    SkoeWindow::SkoeWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name}
    {
        initWindow();
    }

    SkoeWindow::~SkoeWindow()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void SkoeWindow::initWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    void SkoeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if(glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window surface ");
        }
    }

    void SkoeWindow::framebufferResizeCallback(GLFWwindow *window, int width, int height)
    {
        auto skoeWindow = reinterpret_cast<SkoeWindow *>(glfwGetWindowUserPointer(window));
        skoeWindow->framebufferResized = true;
        skoeWindow->width = width;
        skoeWindow->height = height;

    }
}