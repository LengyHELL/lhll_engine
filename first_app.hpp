#ifndef FIRST_APP_HPP
#define FIRST_APP_HPP

#include "lhll_device.hpp"
#include "lhll_game_object.hpp"
#include "lhll_window.hpp"
#include "lhll_renderer.hpp"
#include "lhll_descriptors.hpp"

#include <memory>
#include <vector>

namespace lhll {
  class FirstApp {
  public:
    static constexpr int WIDTH = 1200;
    static constexpr int HEIGHT = 900;

    FirstApp();
    ~FirstApp();

    FirstApp(const FirstApp&) = delete;
    FirstApp& operator=(const FirstApp&) = delete;

    void run();
  private:
    void loadGameObjects();

    LhllWindow lhllWindow{WIDTH, HEIGHT, "Vulkan engine"};
    LhllDevice lhllDevice{lhllWindow};
    LhllRenderer lhllRenderer{lhllWindow, lhllDevice};

    std::unique_ptr<LhllDescriptorPool> globalPool{};
    LhllGameObject::Map gameObjects;
  };
}

#endif
