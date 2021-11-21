#ifndef LHLL_RENDERER_HPP
#define LHLL_RENDERER_HPP

#include "lhll_device.hpp"
#include "lhll_swap_chain.hpp"
#include "lhll_window.hpp"

#include <cassert>
#include <memory>
#include <vector>

namespace lhll {
  class LhllRenderer {
  public:
    LhllRenderer(LhllWindow& window, LhllDevice& device);
    ~LhllRenderer();

    LhllRenderer(const LhllRenderer&) = delete;
    LhllRenderer& operator=(const LhllRenderer&) = delete;

    VkRenderPass getSwapChainRenderPass() const { return lhllSwapChain->getRenderPass(); }
    bool isFrameInProgress() const { return isFrameStarted; }

    VkCommandBuffer getCurrentCommandBuffer() const {
      assert(isFrameStarted && "Cannot get command buffer when frame is not in progress");
      return commandBuffers[currentImageIndex];
    }

    VkCommandBuffer beginFrame();
    void endFrame();
    void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
    void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

  private:
    void createCommandBuffers();
    void freeCommandBuffers();
    void recreateSwapChain();

    LhllWindow& lhllWindow;
    LhllDevice& lhllDevice;
    std::unique_ptr<LhllSwapChain> lhllSwapChain;
    std::vector<VkCommandBuffer> commandBuffers;

    uint32_t currentImageIndex;
    bool isFrameStarted{false};
  };
}

#endif
