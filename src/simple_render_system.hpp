#ifndef SIMPLE_RENDER_SYSTEM_HPP
#define SIMPLE_RENDER_SYSTEM_HPP

#include "lhll_camera.hpp"
#include "lhll_device.hpp"
#include "lhll_frame_info.hpp"
#include "lhll_game_object.hpp"
#include "lhll_pipeline.hpp"

#include <memory>
#include <vector>

namespace lhll {
  class SimpleRenderSystem {
  public:
    SimpleRenderSystem(LhllDevice& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem&) = delete;
    SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

    void renderGameObjects(FrameInfo& frameInfo);

  private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    LhllDevice& lhllDevice;

    std::unique_ptr<LhllPipeline> lhllPipeline;
    VkPipelineLayout pipelineLayout;
  };
}

#endif
