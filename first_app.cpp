#include "first_app.hpp"

namespace lhll {

  FirstApp::FirstApp() {
    createPipelineLayout();
    createPipeline();
    createCommandBuffers();
  }

  FirstApp::~FirstApp() {
    vkDestroyPipelineLayout(lhllDevice.device(), pipelineLayout, nullptr);
  }

  void FirstApp::run() {

    while (!lhllWindow.shouldClose()) {
      glfwPollEvents();
    }
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
    auto pipelineConfig = LhllPipeline::defaultPipelineConfigInfo(lhllSwapChain.width(), lhllSwapChain.height());
    pipelineConfig.renderPass = lhllSwapChain.getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    lhllPipeline = std::make_unique<LhllPipeline>(lhllDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
  }

  void FirstApp::createCommandBuffers() {}
  void FirstApp::drawFrame() {}
}
