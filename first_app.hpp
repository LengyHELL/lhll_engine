#ifndef FIRST_APP_HPP
#define FIRST_APP_HPP

#include "lhll_device.hpp"
#include "lhll_pipeline.hpp"
#include "lhll_swap_chain.hpp"
#include "lhll_window.hpp"
#include "lhll_model.hpp"

#include <stdexcept>
#include <memory>
#include <vector>

namespace lhll {
  class FirstApp {
  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    FirstApp();
    ~FirstApp();

    FirstApp(const FirstApp&) = delete;
    FirstApp& operator=(const FirstApp&) = delete;

    void run();
  private:
    void loadModels();
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void drawFrame();

    LhllWindow lhllWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
    LhllDevice lhllDevice{lhllWindow};
    LhllSwapChain lhllSwapChain{lhllDevice, lhllWindow.getExtent()};
    std::unique_ptr<LhllPipeline> lhllPipeline;
    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
    std::unique_ptr<LhllModel> lhllModel;
  };
}

#endif
