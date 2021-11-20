#ifndef LHLL_WINDOW_HPP
#define LHLL_WINDOW_HPP

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace lhll {
  class LhllWindow {
  public:
    LhllWindow(int w, int h, std::string name);
    ~LhllWindow();

    LhllWindow(const LhllWindow&) = delete;
    LhllWindow &operator=(const LhllWindow&) = delete;

    bool shouldClose() { return glfwWindowShouldClose(window); }
    VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }

    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

  private:
    void initWindow();

    const int width;
    const int height;

    std::string windowName;
    GLFWwindow *window;
  };
}

#endif
