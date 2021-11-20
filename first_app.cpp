#include "first_app.hpp"

namespace lhll {

  void FirstApp::run() {

    while (!lhllWindow.shouldClose()) {
      glfwPollEvents();
    }
  }
}
