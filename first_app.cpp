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

  std::unique_ptr<LhllModel> createCubeModel(LhllDevice& device, glm::vec3 offset) {
    LhllModel::Builder modelBuilder{};
    modelBuilder.vertices = {
        // left face (white)
        {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
        {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

        // right face (yellow)
        {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

        // top face (orange, remember y axis points down)
        {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

        // bottom face (red)
        {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
        {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

        // nose face (blue)
        {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

        // tail face (green)
        {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
    };

    for (auto& v : modelBuilder.vertices) {
      v.position += offset;
    }

    modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                            12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

    return std::make_unique<LhllModel>(device, modelBuilder);
  }

  void FirstApp::loadGameObjects() {
    std::shared_ptr<LhllModel> lhllModel = createCubeModel(lhllDevice, {0.0f, 0.0f, 0.0f});

    auto cube = LhllGameObject::createGameObject();
    cube.model = lhllModel;
    cube.transform.translation = {0.0f, 0.0f, 2.5f};
    cube.transform.scale = {0.5f, 0.5f, 0.5f};

    gameObjects.push_back(std::move(cube));
  }
}
