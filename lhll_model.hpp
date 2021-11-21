#ifndef GUARD
#define GUARD

#include "lhll_device.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace lhll {
  class LhllModel {
  public:

    struct Vertex {
      glm::vec3 position;
      glm::vec3 color;
      static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
      static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
    };

    LhllModel(LhllDevice &device, const std::vector<Vertex> &vertices);
    ~LhllModel();

    LhllModel(const LhllModel&) = delete;
    LhllModel& operator=(const LhllModel&) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

  private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);

    LhllDevice& lhllDevice;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t vertexCount;
  };
}

#endif
