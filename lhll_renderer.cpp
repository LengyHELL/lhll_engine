#include "lhll_renderer.hpp"

#include <array>
#include <cassert>
#include <stdexcept>

namespace lhll {

  LhllRenderer::LhllRenderer(LhllWindow& window, LhllDevice& device) : lhllWindow{window}, lhllDevice{device} {
    recreateSwapChain();
    createCommandBuffers();
  }

  LhllRenderer::~LhllRenderer() {
    freeCommandBuffers();
  }

  void LhllRenderer::recreateSwapChain() {
    auto extent = lhllWindow.getExtent();
    while (extent.width == 0 || extent.height == 0) {
      extent = lhllWindow.getExtent();
      glfwWaitEvents();
    }

    vkDeviceWaitIdle(lhllDevice.device());

    if (lhllSwapChain == nullptr) {
      lhllSwapChain = std::make_unique<LhllSwapChain>(lhllDevice, extent);
    }
    else {
      std::shared_ptr<LhllSwapChain> oldSwapChain = std::move(lhllSwapChain);
      lhllSwapChain = std::make_unique<LhllSwapChain>(lhllDevice, extent, oldSwapChain);

      if (!oldSwapChain->compareSwapFormats(*lhllSwapChain.get())) {
        throw std::runtime_error("Swap chain image or depth format has changed");
      }
    }
    // we'll come back later
  }


  void LhllRenderer::createCommandBuffers() {
    commandBuffers.resize(LhllSwapChain::MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = lhllDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(lhllDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate command buffers!");
    }
  }

  void LhllRenderer::freeCommandBuffers() {
    vkFreeCommandBuffers(lhllDevice.device(), lhllDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    commandBuffers.clear();
  }

  VkCommandBuffer LhllRenderer::beginFrame() {
    assert(!isFrameStarted && "Can't call beginFrame while already in progress");

    auto result = lhllSwapChain->acquireNextImage(&currentImageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      recreateSwapChain();
      return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image!");
    }

    isFrameStarted = true;

    auto commandBuffer = getCurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    return commandBuffer;
  }

  void LhllRenderer::endFrame() {
    assert(isFrameStarted && "Can't call endFrame when frame is not in progress");

    auto commandBuffer = getCurrentCommandBuffer();
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }

    auto result = lhllSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lhllWindow.wasWindowResized()) {
      lhllWindow.resetWindowResizedFlag();
      recreateSwapChain();
    }
    else if(result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image!");
    }

    isFrameStarted = false;
    currentFrameIndex = (currentFrameIndex + 1) % LhllSwapChain::MAX_FRAMES_IN_FLIGHT;
  }

  void LhllRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(isFrameStarted && "Can't call beginSwapChainRenderPass when frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't begin render pass on command buffer from a different frame");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = lhllSwapChain->getRenderPass();
    renderPassInfo.framebuffer = lhllSwapChain->getFrameBuffer(currentImageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = lhllSwapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(lhllSwapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(lhllSwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, lhllSwapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
  }

  void LhllRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
    assert(isFrameStarted && "Can't call endSwapChainRenderPass when frame is not in progress");
    assert(commandBuffer == getCurrentCommandBuffer() && "Can't end render pass on command buffer from a different frame");

    vkCmdEndRenderPass(commandBuffer);
  }

}
