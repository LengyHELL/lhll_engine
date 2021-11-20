#ifndef LHLL_WINDOW_H
#define LHLL_WINDOW_H

#define GLFW_INCLUED_VULKAN
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

  private:
    void initWindow();

    const int width;
    const int height;

    std::string windowName;
    GLFWwindow *window;
  };
}

#endif
