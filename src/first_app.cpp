#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
#include "lhll_buffer.hpp"
#include "lhll_camera.hpp"
#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <chrono>
#include <cassert>
#include <stdexcept>

#include <iostream>

namespace lhll {
  struct GlobalUbo {
    glm::mat4 projectionView{1.0f};
    glm::vec4 ambientLightColor{1.0f, 1.0f, 1.0f, 0.02f};
    glm::vec3 lightPosition{-1.0f};
    alignas(16) glm::vec4 lightColor{1.0f};
  };

  FirstApp::FirstApp() {
    globalPool = LhllDescriptorPool::Builder(lhllDevice).setMaxSets(LhllSwapChain::MAX_FRAMES_IN_FLIGHT).addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, LhllSwapChain::MAX_FRAMES_IN_FLIGHT).build();
    loadGameObjects();
    }

  FirstApp::~FirstApp() {}

  void FirstApp::run() {
    std::vector<std::unique_ptr<LhllBuffer>> uboBuffers(LhllSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < uboBuffers.size(); i++) {
      uboBuffers[i] = std::make_unique<LhllBuffer>(lhllDevice, sizeof(GlobalUbo), 1, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, lhllDevice.properties.limits.minUniformBufferOffsetAlignment);
      uboBuffers[i]->map();
    }

    auto globalSetLayout = LhllDescriptorSetLayout::Builder(lhllDevice).addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS).build();

    std::vector<VkDescriptorSet> globalDescriptorSets(LhllSwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
      auto bufferInfo = uboBuffers[i]->descriptorInfo();
      LhllDescriptorWriter(*globalSetLayout, *globalPool).writeBuffer(0, &bufferInfo).build(globalDescriptorSets[i]);
    }

    SimpleRenderSystem simpleRenderSystem{lhllDevice, lhllRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
    LhllCamera camera{};
    //camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));
    camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 2.5f));

    auto viewerObject = LhllGameObject::createGameObject();
    viewerObject.transform.translation.z = -2.5;
    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

    glfwSetInputMode(lhllWindow.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    while (!lhllWindow.shouldClose()) {
      double xpos, ypos;
      glfwPollEvents();
      glfwGetCursorPos(lhllWindow.getGLFWwindow(), &xpos, &ypos);

      auto newTime = std::chrono::high_resolution_clock::now();
      float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
      currentTime = newTime;

      cameraController.moveInPlaneXZMouse(lhllWindow.getGLFWwindow(), frameTime, glm::vec2{float(ypos), float(xpos)}, viewerObject);
      camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

      float aspect = lhllRenderer.getAspectRatio();
      camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 100.0f);

      if (auto commandBuffer = lhllRenderer.beginFrame()) {
        int frameIndex = lhllRenderer.getFrameIndex();
        FrameInfo frameInfo{frameIndex, frameTime, commandBuffer, camera, globalDescriptorSets[frameIndex], gameObjects};

        // update systems
        GlobalUbo ubo{};
        ubo.projectionView = camera.getProjection() * camera.getView();
        uboBuffers[frameIndex]->writeToBuffer(&ubo);
        uboBuffers[frameIndex]->flush();

        // render system
        lhllRenderer.beginSwapChainRenderPass(commandBuffer);
        simpleRenderSystem.renderGameObjects(frameInfo);
        lhllRenderer.endSwapChainRenderPass(commandBuffer);
        lhllRenderer.endFrame();
      }
    }

    vkDeviceWaitIdle(lhllDevice.device());
  }

  void FirstApp::loadGameObjects() {
    std::shared_ptr<LhllModel> lhllModel = LhllModel::createModelFromFile(lhllDevice, "models/flat_vase.obj");
    auto flatVase = LhllGameObject::createGameObject();
    flatVase.model = lhllModel;
    flatVase.transform.translation = {-0.5f, 0.5f, 0.0f};
    flatVase.transform.scale = {3.0f, 1.5f, 3.0f};
    gameObjects.emplace(flatVase.getId(), std::move(flatVase));

    lhllModel = LhllModel::createModelFromFile(lhllDevice, "models/smooth_vase.obj");
    auto smoothVase = LhllGameObject::createGameObject();
    smoothVase.model = lhllModel;
    smoothVase.transform.translation = {0.5f, 0.5f, 0.0f};
    smoothVase.transform.scale = {3.0f, 1.5f, 3.0f};
    gameObjects.emplace(smoothVase.getId(), std::move(smoothVase));

    lhllModel = LhllModel::createModelFromFile(lhllDevice, "models/quad.obj");
    auto floor = LhllGameObject::createGameObject();
    floor.model = lhllModel;
    floor.transform.translation = {0.0f, 0.5f, 0.0f};
    floor.transform.scale = {10.0f, 1.0f, 10.0f};
    gameObjects.emplace(floor.getId(), std::move(floor));
  }
}
