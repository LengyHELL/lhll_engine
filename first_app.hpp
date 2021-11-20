#ifndef FIRST_APP_HPP
#define FIRST_APP_HPP

#include "lhll_window.hpp"

namespace lhll {
  class FirstApp {
  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    void run();
  private:
    LhllWindow lhllWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
  };
}

#endif
