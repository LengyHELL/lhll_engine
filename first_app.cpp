#include "first_app.hpp"

#include <array>

namespace lhll {

  FirstApp::FirstApp() {
    loadModels();
    createPipelineLayout();
    recreateSwapChain();
    createCommandBuffers();
  }

  FirstApp::~FirstApp() {
    vkDestroyPipelineLayout(lhllDevice.device(), pipelineLayout, nullptr);
  }

  void FirstApp::run() {

    while (!lhllWindow.shouldClose()) {
      glfwPollEvents();
      drawFrame();
    }

    vkDeviceWaitIdle(lhllDevice.device());
  }

  void lot_triangles(std::vector<LhllModel::Vertex>& vertices, const int& iter = 1) {
    for (int it = 0; it < iter; it++) {
      std::vector<LhllModel::Vertex> temp;
      int size = vertices.size();
      for (int i = 0; i < size; i++) {
        temp.push_back(vertices[i]);
        int start = int(i / 3) * 3;
        for (int j = start; j < start + 3; j++) {
          if (i != j) {
            glm::vec2 a = vertices[i].position;
            glm::vec2 b = vertices[j].position;
            b -= a;
            float len = glm::length(b);
            b = glm::normalize(b);
            b = a + (b * (len / 2));
            temp.push_back({{b.x, b.y}, vertices[j].color});
          }
        }
      }
      vertices = temp;
    }
  }

  void FirstApp::loadModels() {
    std::vector<LhllModel::Vertex> vertices {
      {{  0.0f, -1.0f}, {1.0f, 0.0f, 0.0f}},
      {{  1.0f,  1.0f}, {0.0f, 1.0f, 0.0f}},
      {{ -1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}}
    };

    lot_triangles(vertices, 7);

    lhllModel = std::make_unique<LhllModel>(lhllDevice, vertices);
  }

  void FirstApp::createPipelineLayout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(lhllDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
      throw std::runtime_error("failed to create pipeline layout!");
    }
  }

  void FirstApp::createPipeline() {
    assert(lhllSwapChain != nullptr && "Cannot create pipeline before swap chain");
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");


    PipelineConfigInfo pipelineConfig{};
    LhllPipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = lhllSwapChain->getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    lhllPipeline = std::make_unique<LhllPipeline>(lhllDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
  }

  void FirstApp::recreateSwapChain() {
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
      lhllSwapChain = std::make_unique<LhllSwapChain>(lhllDevice, extent, std::move(lhllSwapChain));
      if (lhllSwapChain->imageCount() != commandBuffers.size()) {
        freeCommandBuffers();
        createCommandBuffers();
      }
    }

    createPipeline();
  }

  void FirstApp::createCommandBuffers() {
    commandBuffers.resize(lhllSwapChain->imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = lhllDevice.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(lhllDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
      throw std::runtime_error("failed to allocate command buffers!");
    }
  }

  void FirstApp::freeCommandBuffers() {
    vkFreeCommandBuffers(lhllDevice.device(), lhllDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
    commandBuffers.clear();
  }

  void FirstApp::recordCommandBuffer(int imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = lhllSwapChain->getRenderPass();
    renderPassInfo.framebuffer = lhllSwapChain->getFrameBuffer(imageIndex);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = lhllSwapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {0.0f, 0.2f, 0.1f, 1.0f};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(lhllSwapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(lhllSwapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, lhllSwapChain->getSwapChainExtent()};
    vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
    vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

    lhllPipeline->bind(commandBuffers[imageIndex]);
    lhllModel->bind(commandBuffers[imageIndex]);
    lhllModel->draw(commandBuffers[imageIndex]);

    vkCmdEndRenderPass(commandBuffers[imageIndex]);
    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }

  void FirstApp::drawFrame() {
    uint32_t imageIndex;
    auto result = lhllSwapChain->acquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
      recreateSwapChain();
      return;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
      throw std::runtime_error("failed to acquire swap chain image!");
    }

    recordCommandBuffer(imageIndex);
    result = lhllSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lhllWindow.wasWindowResized()) {
      lhllWindow.resetWindowResizedFlag();
      recreateSwapChain();
      return;
    }
    if(result != VK_SUCCESS) {
      throw std::runtime_error("failed to present swap chain image!");
    }
  }
}
