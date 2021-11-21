#ifndef LHLL_GAME_OBJECT_HPP
#define LHLL_GAME_OBJECT_HPP

#include "lhll_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>

namespace lhll {
  struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
    glm::vec3 rotation{};

    glm::mat4 mat4() {
      auto transform = glm::translate(glm::mat4{1.0f}, translation);

      transform = glm::rotate(transform, rotation.y, {0.0f, 1.0f, 0.0f});
      transform = glm::rotate(transform, rotation.x, {1.0f, 0.0f, 0.0f});
      transform = glm::rotate(transform, rotation.z, {0.0f, 0.0f, 1.0f});

      transform = glm::scale(transform, scale);
      return transform;
    }
  };

  class LhllGameObject {
  public:
    using id_t = unsigned int;

    static LhllGameObject createGameObject() {
      static id_t currentId = 0;
      return LhllGameObject{currentId++};
    }

    LhllGameObject(const LhllGameObject&) = delete;
    LhllGameObject& operator=(const LhllGameObject&) = delete;
    LhllGameObject(LhllGameObject&&) = default;
    LhllGameObject& operator=(LhllGameObject&&) = default;

    id_t getId() { return id; }

    std::shared_ptr<LhllModel> model{};
    glm::vec3 color{};
    TransformComponent transform{};

  private:
    LhllGameObject(id_t objId) : id{objId} {}
    id_t id;
  };
}

#endif
