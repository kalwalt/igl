/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <igl/Device.h>
#include <igl/Shader.h>

#include <algorithm>
#include <utility>

namespace igl {

void IDevice::beginScope() {
  ++scopeDepth_;
}

void IDevice::endScope() {
  --scopeDepth_;
}

bool IDevice::verifyScope() {
  return scopeDepth_ > 0;
}

void IDevice::updateSurface(void* nativeWindowType) {}

TextureDesc IDevice::sanitize(const TextureDesc& desc) const {
  TextureDesc sanitized = desc;
  if (desc.width == 0 || desc.height == 0 || desc.depth == 0 || desc.numLayers == 0 ||
      desc.numSamples == 0 || desc.numMipLevels == 0) {
    sanitized.width = std::max(sanitized.width, static_cast<size_t>(1));
    sanitized.height = std::max(sanitized.height, static_cast<size_t>(1));
    sanitized.depth = std::max(sanitized.depth, static_cast<size_t>(1));
    sanitized.numLayers = std::max(sanitized.numLayers, static_cast<size_t>(1));
    sanitized.numSamples = std::max(sanitized.numSamples, 1u);
    sanitized.numMipLevels = std::max(sanitized.numMipLevels, 1u);
    IGL_LOG_ERROR(
        "width (%d), height (%d), depth (%d), numLayers (%d), numSamples (%d) and numMipLevels "
        "(%d) should be at least 1.\n",
        desc.width,
        desc.height,
        desc.depth,
        desc.numLayers,
        desc.numSamples,
        desc.numMipLevels);
  }

  return sanitized;
}

Color IDevice::backendDebugColor() const noexcept {
  switch (getBackendType()) {
  case BackendType::OpenGL:
    return {1.f, 1.f, 0.f, 1.f};
  case BackendType::Metal:
    return {1.f, 0.f, 1.f, 1.f};
  case BackendType::Vulkan:
    return {0.f, 1.f, 1.f, 1.f};
  // @fb-only
    // @fb-only
  }
  IGL_UNREACHABLE_RETURN(Color(0.f, 0.f, 0.f, 0.f))
}

DeviceScope::DeviceScope(IDevice& device) : device_(device) {
  device_.beginScope();
}

DeviceScope::~DeviceScope() {
  device_.endScope();
}

} // namespace igl
