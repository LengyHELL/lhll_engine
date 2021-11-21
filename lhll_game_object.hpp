#ifndef LHLL_GAME_OBJECT_HPP
#define LHLL_GAME_OBJECT_HPP

#include "lhll_model.hpp"

#include <memory>

namespace lhll {
  struct Transform2dComponent {
    glm::vec2 translation{};
    glm::vec2 scale{1.0f, 1.0f};
    float rotation;

    glm::mat2 mat2() {
      const float s = glm::sin(rotation);
      const float c = glm::cos(rotation);
      glm::mat2 rotateMat{
        {c, s},
        {-s, c}
      };
      glm::mat2 scaleMat{
        {scale.x, 0.0f},
        {0.0f, scale.y}
      };
      return rotateMat * scaleMat;
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
    Transform2dComponent transform2D{};

  private:
    LhllGameObject(id_t objId) : id{objId} {}
    id_t id;
  };
}

#endif
