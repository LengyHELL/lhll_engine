#include "keyboard_movement_controller.hpp"

namespace lhll {
  void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, LhllGameObject& gameObject) {
    glm::vec3 rotate{0};

    if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) rotate.y += 1.0f;
    if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) rotate.y -= 1.0f;
    if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) rotate.x += 1.0f;
    if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) rotate.x -= 1.0f;

    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
      gameObject.transform.rotation += lookSpeed * dt * glm::normalize(rotate);
    }

    gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

    float yaw = gameObject.transform.rotation.y;
    const glm::vec3 forwardDir{sin(yaw), 0.0f, cos(yaw)};
    const glm::vec3 rightDir{forwardDir.z, 0.0f, -forwardDir.x};
    const glm::vec3 upDir{0.0f, -1.0f, 0.0f};

    glm::vec3 moveDir{0.0f};
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
      gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
    }
  }

  void KeyboardMovementController::moveInPlaneXZMouse(GLFWwindow* window, float dt, glm::vec2 mousePosition, LhllGameObject& gameObject) {

    if ((glfwGetKey(window, keys.enableCursor) == GLFW_PRESS) && (!setCursorLock)) {
      setCursorLock = true;
      setCursor = true;
    }
    if (glfwGetKey(window, keys.enableCursor) != GLFW_PRESS) { setCursorLock = false; }

    if (setCursor) {
      setCursor = false;
      cursorEnabled = !cursorEnabled;
      if (cursorEnabled) { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); }
      else { glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); }
    }

    mouseMovement = mousePosition - mousePrevious;
    mousePrevious = mousePosition;

    glm::vec3 rotate{0};
    if (!cursorEnabled) {
      rotate.x -= mouseMovement.x;
      rotate.y += mouseMovement.y;
    }
    if (glm::dot(rotate, rotate) > std::numeric_limits<float>::epsilon()) {
      gameObject.transform.rotation += lookSpeed * dt * rotate;
    }


    gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x, -1.5f, 1.5f);
    gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

    rotate = gameObject.transform.rotation;
    const glm::vec3 forwardDir{sin(rotate.y), tan(-rotate.x), cos(rotate.y)};
    const glm::vec3 rightDir{forwardDir.z, 0.0f, -forwardDir.x};
    const glm::vec3 upDir{0.0f, -1.0f, 0.0f};

    glm::vec3 moveDir{0.0f};
    if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
    if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

    if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
      gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
    }
  }

}
