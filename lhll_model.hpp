#ifndef LHLL_MODEL_HPP
#define LHLL_MODEL_HPP

#include "lhll_device.hpp"
#include "lhll_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <memory>
#include <vector>

namespace lhll {
  class LhllModel {
  public:

    struct Vertex {
      glm::vec3 position{};
      glm::vec3 color{};
      glm::vec3 normal{};
      glm::vec2 uv{};

      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

      bool operator==(const Vertex& other) const {
        return (position == other.position) && (color == other.color) && (normal == other.normal) && (uv == other.uv);
      }
    };

    struct Builder {
      std::vector<Vertex> vertices{};
      std::vector<uint32_t> indices{};

      void loadModel(const std::string& filepath);
    };

    LhllModel(LhllDevice& device, const LhllModel::Builder& builder);
    ~LhllModel();

    LhllModel(const LhllModel&) = delete;
    LhllModel& operator=(const LhllModel&) = delete;

    static std::unique_ptr<LhllModel> createModelFromFile(LhllDevice& device, const std::string& filepath);

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

  private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);
    void createIndexBuffers(const std::vector<uint32_t> &indices);

    LhllDevice& lhllDevice;

    std::unique_ptr<LhllBuffer> vertexBuffer;
    uint32_t vertexCount;

    bool hasIndexBuffer = false;
    std::unique_ptr<LhllBuffer> indexBuffer;
    uint32_t indexCount;
  };
}

#endif
