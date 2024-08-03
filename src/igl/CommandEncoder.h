/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <string>

#include <igl/Common.h>

namespace igl {

class ICommandBuffer;
class IDevice;

class IBuffer;
class IRenderPipelineState;
class ISamplerState;
class ITexture;

/**
 * Dependencies are used to issue proper memory barriers for external resources, such as textures
 * modified by non-IGL code (Skia, Qt, etc), and synchronize between graphics and compute pipelines.
 */
struct Dependencies {
  static constexpr uint32_t IGL_MAX_TEXTURE_DEPENDENCIES = 8;
  static constexpr uint32_t IGL_MAX_BUFFER_DEPENDENCIES = 8;
  // Note: please ensure that both arrays are dense, meaning that processing will halt immediately
  // if any NULL texture or buffer is encountered.
  ITexture* IGL_NULLABLE textures[IGL_MAX_TEXTURE_DEPENDENCIES] = {};
  IBuffer* IGL_NULLABLE buffers[IGL_MAX_BUFFER_DEPENDENCIES] = {};
  const Dependencies* IGL_NULLABLE next = nullptr; // optional extra dependencies
};

/**
 * A BindGroup represents a set of resources bound to a command encoder.
 * It is a replacement of the old OpenGL-style binding model where individual resources are bound
 * using multiple calls to bindTexture(...), bindSamplerState(), and bindBuffer(...).
 */
struct BindGroupTextureDesc {
  std::shared_ptr<ITexture> textures[IGL_TEXTURE_SAMPLERS_MAX] = {};
  std::shared_ptr<ISamplerState> samplers[IGL_TEXTURE_SAMPLERS_MAX] = {};
  std::string debugName;
};
struct BindGroupBufferDesc {
  std::shared_ptr<IBuffer> buffers[IGL_UNIFORM_BLOCKS_BINDING_MAX] = {};
  size_t offset[IGL_UNIFORM_BLOCKS_BINDING_MAX] = {};
  size_t size[IGL_UNIFORM_BLOCKS_BINDING_MAX] = {}; // 0 means the remaining size from `offset` to
                                                    // the end of the buffer
  uint32_t isDynamicBufferMask = 0; // one bit per each buffer
  std::string debugName;
};

class ICommandEncoder {
 public:
  explicit ICommandEncoder(const std::shared_ptr<ICommandBuffer>& commandBuffer) :
    commandBuffer_(commandBuffer) {}

  virtual ~ICommandEncoder() = default;

  /**
   * Declares the end of commands generated by this instance of the encoder.
   *
   * After endEncoding() is called, the encoder is invalid. You must create
   * another encoder to submit more commands.
   */
  virtual void endEncoding() = 0;

  /**
   * Pushes a debug label onto a stack of debug string labels into the captured frame data.
   *
   * If supported by the backend GPU driver, this allows you to easily associate subsequent
   * commands in the captured call stack with this label.
   *
   * When all commands for this label have been sent to the encoder, call popDebugGroupLabel()
   * to pop the label off the stack.
   */
  virtual void pushDebugGroupLabel(const char* IGL_NONNULL label,
                                   const igl::Color& color = igl::Color(1, 1, 1, 1)) const = 0;

  /**
   * Inserts a debug label into the captured frame data.
   *
   * If supported by the backend GPU driver, this allows you to easily insert a label into the
   * call stack of the captured frame data.
   */
  virtual void insertDebugEventLabel(const char* IGL_NONNULL label,
                                     const igl::Color& color = igl::Color(1, 1, 1, 1)) const = 0;

  /**
   * Pops a most recent debug label off a stack of debug string labels.
   *
   * This should be preceded by pushDebugGroupLabel().
   */
  virtual void popDebugGroupLabel() const = 0;

  ICommandBuffer& getCommandBuffer() {
    IGL_ASSERT(commandBuffer_);
    return *commandBuffer_;
  }

  [[nodiscard]] const std::shared_ptr<ICommandBuffer>& getCommandBufferPtr() const {
    IGL_ASSERT(commandBuffer_);
    return commandBuffer_;
  }

 private:
  std::shared_ptr<ICommandBuffer> commandBuffer_;
};

} // namespace igl

#if IGL_DEBUG && !defined(IGL_DISABLE_DEBUG_ENCODER_LABEL)
#define IGL_DEBUG_ENCODER_LABEL_START(encoder, x) encoder.pushDebugGroupLabel(#x)
#define IGL_DEBUG_ENCODER_LABEL_EVENT(encoder, x) encoder.insertDebugEventLabel(#x)
#define IGL_DEBUG_ENCODER_LABEL_END(encoder) encoder.popDebugGroupLabel()
#else
#define IGL_DEBUG_ENCODER_LABEL_START(encoder, x)
#define IGL_DEBUG_ENCODER_LABEL_EVENT(encoder, x)
#define IGL_DEBUG_ENCODER_LABEL_END(encoder)
#endif // IGL_DEBUG && !defined(IGL_DISABLE_DEBUG_ENCODER_LABEL)
