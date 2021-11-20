#ifndef LHLL_PIPELINE_HPP
#define LHLL_PIPELINE_HPP

#include <string>
#include <vector>

namespace lhll {
  class LhllPipeline {
  public:
    LhllPipeline(const std::string& vertFilepath, const std::string& fragFilepath);

  private:
    static std::vector<char> readFile(const std::string& filepath);

    void createGraphicsPipeline(const std::string& vertFilepath, const std::string& fragFilepath);
  };
}

#endif
