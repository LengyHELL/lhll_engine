#include "first_app.hpp"

#include "simple_render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <stdexcept>

namespace lhll {

  FirstApp::FirstApp() { loadGameObjects(); }

  FirstApp::~FirstApp() {}

  void FirstApp::run() {
    SimpleRenderSystem simpleRenderSystem{lhllDevice, lhllRenderer.getSwapChainRenderPass()};

    while (!lhllWindow.shouldClose()) {
      glfwPollEvents();

      if (auto commandBuffer = lhllRenderer.beginFrame()) {
        lhllRenderer.beginSwapChainRenderPass(commandBuffer);
        simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
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
      triangle.transform2d.translation.x = 0.0f;
      triangle.transform2d.scale = {0.1f * i, 0.1f * i};
      triangle.transform2d.rotation = 0.25 * glm::two_pi<float>();

      gameObjects.push_back(std::move(triangle));
    }
  }

}
