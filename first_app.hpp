#ifndef FIRST_APP_HPP
#define FIRST_APP_HPP

#include "lhll_device.hpp"
#include "lhll_game_object.hpp"
#include "lhll_pipeline.hpp"
#include "lhll_window.hpp"
#include "lhll_renderer.hpp"

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
    void createPipelineLayout();
    void createPipeline();
    void renderGameObjects(VkCommandBuffer commandBuffer);

    LhllWindow lhllWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
    LhllDevice lhllDevice{lhllWindow};
    LhllRenderer lhllRenderer{lhllWindow, lhllDevice};

    std::unique_ptr<LhllPipeline> lhllPipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<LhllGameObject> gameObjects;
  };
}

#endif
