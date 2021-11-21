#include "first_app.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <stdexcept>

namespace lhll {

  struct SimplePushConstantData {
    glm::mat2 transform{1.f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
  };

  FirstApp::FirstApp() {
    loadGameObjects();
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

  void FirstApp::loadGameObjects() {
    std::vector<LhllModel::Vertex> vertices {
      {{  0.0f, -0.6f}, {0.6f, 0.0f, 0.0f}},
      {{  0.6f,  0.6f}, {0.0f, 0.6f, 0.0f}},
      {{ -0.6f,  0.6f}, {0.0f, 0.0f, 0.6f}}
    };

    lot_triangles(vertices, 0);

    for (float i = 0; i < 50; i += 0.1) {
      auto lhllModel = std::make_shared<LhllModel>(lhllDevice, vertices);

      auto triangle = LhllGameObject::createGameObject();
      triangle.model = lhllModel;
      triangle.color = {0.1f * i, 0.2f * (i / 2), 0.1f};
      triangle.transform2D.translation.x = 0.2f;
      triangle.transform2D.scale = {0.1f * i, 0.1f * i};
      triangle.transform2D.rotation = 0.25 * glm::two_pi<float>();

      gameObjects.push_back(std::move(triangle));
    }
  }

  void FirstApp::createPipelineLayout() {

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(SimplePushConstantData);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

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
    clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
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

    renderGameObjects(commandBuffers[imageIndex]);

    vkCmdEndRenderPass(commandBuffers[imageIndex]);
    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
  }

  void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer) {
    // update
    int i = 0;
    for (auto& obj : gameObjects) {
      i += 1;
      obj.transform2D.rotation = glm::mod<float>(obj.transform2D.rotation + 0.0001f * i, 2.0f * glm::pi<float>());
    }

    // render
    lhllPipeline->bind(commandBuffer);
    for (auto& obj : gameObjects) {
      SimplePushConstantData push{};
      push.offset = obj.transform2D.translation;
      push.color = obj.color;
      push.transform = obj.transform2D.mat2();

      vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(SimplePushConstantData), &push);
      obj.model->bind(commandBuffer);
      obj.model->draw(commandBuffer);
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
