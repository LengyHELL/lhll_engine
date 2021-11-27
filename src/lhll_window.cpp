#include "lhll_window.hpp"

#include <stdexcept>

namespace lhll {
  LhllWindow::LhllWindow(int w, int h, std::string name) : width{w}, height{h}, windowName{name} {
    initWindow();
  }

  LhllWindow::~LhllWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
  }

  void LhllWindow::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
  }

  void LhllWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
      throw std::runtime_error("failed to create window surface");
    }
  }

  void LhllWindow::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    auto lhllWindow = reinterpret_cast<LhllWindow*>(glfwGetWindowUserPointer(window));
    lhllWindow->framebufferResized = true;
    lhllWindow->width = width;
    lhllWindow->height = height;

  }

}
