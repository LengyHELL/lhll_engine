#include "lhll_window.hpp"

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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
  }
}
