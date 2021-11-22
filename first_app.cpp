#include "first_app.hpp"

#include "keyboard_movement_controller.hpp"
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
  FirstApp::FirstApp() { loadGameObjects(); }

  FirstApp::~FirstApp() {}

  void FirstApp::run() {
    SimpleRenderSystem simpleRenderSystem{lhllDevice, lhllRenderer.getSwapChainRenderPass()};
    LhllCamera camera{};
    //camera.setViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));
    camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 2.5f));

    auto viewerObject = LhllGameObject::createGameObject();
    KeyboardMovementController cameraController{};

    auto currentTime = std::chrono::high_resolution_clock::now();

    glfwSetInputMode(lhllWindow.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwRawMouseMotionSupported()) {
      glfwSetInputMode(lhllWindow.getGLFWwindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

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
        // update systems

        // render system
        lhllRenderer.beginSwapChainRenderPass(commandBuffer);
        simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
        lhllRenderer.endSwapChainRenderPass(commandBuffer);
        lhllRenderer.endFrame();
      }
    }

    vkDeviceWaitIdle(lhllDevice.device());
  }

  void FirstApp::loadGameObjects() {
    std::shared_ptr<LhllModel> lhllModel = LhllModel::createModelFromFile(lhllDevice, "models/smooth_vase.obj");

    auto gameObj = LhllGameObject::createGameObject();
    gameObj.model = lhllModel;
    gameObj.transform.translation = {0.0f, 0.0f, 2.5f};
    gameObj.transform.scale = glm::vec3{3.0f};

    gameObjects.push_back(std::move(gameObj));
  }
}
