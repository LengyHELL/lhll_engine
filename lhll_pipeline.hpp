#ifndef LHLL_PIPELINE_HPP
#define LHLL_PIPELINE_HPP

#include "lhll_device.hpp"

#include <string>
#include <vector>

namespace lhll {
  struct PipelineConfigInfo {};
  class LhllPipeline {
  public:
    LhllPipeline(LhllDevice& device, const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);
    ~LhllPipeline() {}

    LhllPipeline(const LhllPipeline&) = delete;
    void operator=(const LhllPipeline&) = delete;

    static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

  private:
    static std::vector<char> readFile(const std::string& filepath);

    void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath, const PipelineConfigInfo& configInfo);

    void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

    LhllDevice& lhllDevice;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
  };
}

#endif
