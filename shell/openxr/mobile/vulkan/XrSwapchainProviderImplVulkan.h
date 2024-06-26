/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <shell/openxr/XrPlatform.h>
#include <shell/openxr/impl/XrSwapchainProviderImpl.h>

#include <igl/vulkan/VulkanTexture.h>

namespace igl::shell::openxr::mobile {
class XrSwapchainProviderImplVulkan final : public impl::XrSwapchainProviderImpl {
 public:
  [[nodiscard]] int64_t preferredColorFormat() const noexcept final {
    return VK_FORMAT_R8G8B8A8_SRGB;
  }
  [[nodiscard]] int64_t preferredDepthFormat() const noexcept final {
    return VK_FORMAT_D24_UNORM_S8_UINT;
  }

  void enumerateImages(igl::IDevice& device,
                       XrSwapchain colorSwapchain,
                       XrSwapchain depthSwapchain,
                       const impl::SwapchainImageInfo& swapchainImageInfo,
                       uint8_t numViews) noexcept final;

  [[nodiscard]] igl::SurfaceTextures getSurfaceTextures(
      igl::IDevice& device,
      XrSwapchain colorSwapchain,
      XrSwapchain depthSwapchain,
      const impl::SwapchainImageInfo& swapchainImageInfo,
      uint8_t numViews) noexcept final;

 private:
  std::vector<std::shared_ptr<igl::vulkan::VulkanTexture>> vulkanColorTextures_;
  std::vector<std::shared_ptr<igl::vulkan::VulkanTexture>> vulkanDepthTextures_;
};
} // namespace igl::shell::openxr::mobile
