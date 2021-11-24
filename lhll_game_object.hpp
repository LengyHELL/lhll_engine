#ifndef LHLL_GAME_OBJECT_HPP
#define LHLL_GAME_OBJECT_HPP

#include "lhll_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace lhll {
  struct TransformComponent {
    glm::vec3 translation{};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
    glm::vec3 rotation{};

    // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
    // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
    // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
    glm::mat4 mat4();
    glm::mat3 normalMatrix();
  };

  class LhllGameObject {
  public:
    using id_t = unsigned int;
    using Map = std::unordered_map<id_t, LhllGameObject>;

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
