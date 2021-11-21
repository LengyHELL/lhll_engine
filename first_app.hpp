#ifndef FIRST_APP_HPP
#define FIRST_APP_HPP

#include "lhll_device.hpp"
#include "lhll_game_object.hpp"
#include "lhll_pipeline.hpp"
#include "lhll_swap_chain.hpp"
#include "lhll_window.hpp"

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
    void createCommandBuffers();
    void freeCommandBuffers();
    void drawFrame();
    void recreateSwapChain();
    void recordCommandBuffer(int imageIndex);
    void renderGameObjects(VkCommandBuffer commandBuffer);

    LhllWindow lhllWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
    LhllDevice lhllDevice{lhllWindow};
    std::unique_ptr<LhllSwapChain> lhllSwapChain;
    std::unique_ptr<LhllPipeline> lhllPipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
    std::vector<LhllGameObject> gameObjects;
  };
}

#endif
