#ifndef KEYBOARD_MOVEMENT_CONTROLLER_HPP
#define KEYBOARD_MOVEMENT_CONTROLLER_HPP

#include "lhll_game_object.hpp"
#include "lhll_window.hpp"

namespace lhll {
  class KeyboardMovementController {
  public:
    struct KeyMappings {
      int moveLeft = GLFW_KEY_A;
      int moveRight = GLFW_KEY_D;
      int moveForward = GLFW_KEY_W;
      int moveBackward = GLFW_KEY_S;
      int moveUp = GLFW_KEY_E;
      int moveDown = GLFW_KEY_Q;
      int lookLeft = GLFW_KEY_LEFT;
      int lookRight = GLFW_KEY_RIGHT;
      int lookUp = GLFW_KEY_UP;
      int lookDown = GLFW_KEY_DOWN;
    };

    void moveInPlaneXZ(GLFWwindow* window, float dt, LhllGameObject& gameObject);
    void moveInPlaneXZMouse(GLFWwindow* window, float dt, glm::vec2 mousePosition, LhllGameObject& gameObject);

    KeyMappings keys{};
    float moveSpeed{3.0f};
    float lookSpeed{0.5f};

    glm::vec2 mousePrevious{0.0f};
    glm::vec2 mouseMovement{0.0f};
  };
}

#endif
