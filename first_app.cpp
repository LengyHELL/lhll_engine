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
    createPipeline();
  }

  FirstApp::~FirstApp() {
    vkDestroyPipelineLayout(lhllDevice.device(), pipelineLayout, nullptr);
  }

  void FirstApp::run() {

    while (!lhllWindow.shouldClose()) {
      glfwPollEvents();

      if (auto commandBuffer = lhllRenderer.beginFrame()) {
        lhllRenderer.beginSwapChainRenderPass(commandBuffer);
        renderGameObjects(commandBuffer);
        lhllRenderer.endSwapChainRenderPass(commandBuffer);
        lhllRenderer.endFrame();
      }
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
    assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");


    PipelineConfigInfo pipelineConfig{};
    LhllPipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = lhllRenderer.getSwapChainRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    lhllPipeline = std::make_unique<LhllPipeline>(lhllDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
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

}
