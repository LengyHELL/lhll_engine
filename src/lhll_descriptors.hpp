#ifndef LHLL_DESCRIPTORS_HPP
#define LHLL_DESCRIPTORS_HPP

#include "lhll_device.hpp"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace lhll {

class LhllDescriptorSetLayout {
 public:
  class Builder {
   public:
    Builder(LhllDevice &lhllDevice) : lhllDevice{lhllDevice} {}

    Builder &addBinding(
        uint32_t binding,
        VkDescriptorType descriptorType,
        VkShaderStageFlags stageFlags,
        uint32_t count = 1);
    std::unique_ptr<LhllDescriptorSetLayout> build() const;

   private:
    LhllDevice &lhllDevice;
    std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
  };

  LhllDescriptorSetLayout(
      LhllDevice &lhllDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
  ~LhllDescriptorSetLayout();
  LhllDescriptorSetLayout(const LhllDescriptorSetLayout &) = delete;
  LhllDescriptorSetLayout &operator=(const LhllDescriptorSetLayout &) = delete;

  VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

 private:
  LhllDevice &lhllDevice;
  VkDescriptorSetLayout descriptorSetLayout;
  std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

  friend class LhllDescriptorWriter;
};

class LhllDescriptorPool {
 public:
  class Builder {
   public:
    Builder(LhllDevice &lhllDevice) : lhllDevice{lhllDevice} {}

    Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
    Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
    Builder &setMaxSets(uint32_t count);
    std::unique_ptr<LhllDescriptorPool> build() const;

   private:
    LhllDevice &lhllDevice;
    std::vector<VkDescriptorPoolSize> poolSizes{};
    uint32_t maxSets = 1000;
    VkDescriptorPoolCreateFlags poolFlags = 0;
  };

  LhllDescriptorPool(
      LhllDevice &lhllDevice,
      uint32_t maxSets,
      VkDescriptorPoolCreateFlags poolFlags,
      const std::vector<VkDescriptorPoolSize> &poolSizes);
  ~LhllDescriptorPool();
  LhllDescriptorPool(const LhllDescriptorPool &) = delete;
  LhllDescriptorPool &operator=(const LhllDescriptorPool &) = delete;

  bool allocateDescriptorSet(
      const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

  void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

  void resetPool();

 private:
  LhllDevice &lhllDevice;
  VkDescriptorPool descriptorPool;

  friend class LhllDescriptorWriter;
};

class LhllDescriptorWriter {
 public:
  LhllDescriptorWriter(LhllDescriptorSetLayout &setLayout, LhllDescriptorPool &pool);

  LhllDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
  LhllDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

  bool build(VkDescriptorSet &set);
  void overwrite(VkDescriptorSet &set);

 private:
  LhllDescriptorSetLayout &setLayout;
  LhllDescriptorPool &pool;
  std::vector<VkWriteDescriptorSet> writes;
};

}  // namespace lhll

#endif