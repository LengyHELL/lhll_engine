#ifndef LHLL_FRAME_INFO_HPP
#define LHLL_FRAME_INFO_HPP

#include "lhll_camera.hpp"

#include <vulkan/vulkan.h>

namespace lhll {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        LhllCamera &camera;
        VkDescriptorSet globalDescriptorSet;
    };
}

#endif