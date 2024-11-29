#pragma once

#include <Render/Private/LogRender.h>
#include <glad/glad.h>

#define GE_RENDER_LOG_OPENGL_ERRORS(Err)                                                   \
  {                                                                                        \
    switch (Err)                                                                           \
    {                                                                                      \
    case GL_CONTEXT_LOST:                                                                  \
      GE_LOG(LogRender, Core::Verbosity::Error, "[OpenGL] CONTEXT_LOST");                  \
    case GL_INVALID_ENUM:                                                                  \
      GE_LOG(LogRender, Core::Verbosity::Error, "[OpenGL] INVALID_ENUM");                  \
    case GL_INVALID_VALUE:                                                                 \
      GE_LOG(LogRender, Core::Verbosity::Error, "[OpenGL] INVALID_VALUE");                 \
    case GL_INVALID_OPERATION:                                                             \
      GE_LOG(LogRender, Core::Verbosity::Error, "[OpenGL] INVALID_OPERATION");             \
    case GL_INVALID_FRAMEBUFFER_OPERATION:                                                 \
      GE_LOG(LogRender, Core::Verbosity::Error, "[OpenGL] INVALID_FRAMEBUFFER_OPERATION"); \
    case GL_OUT_OF_MEMORY:                                                                 \
      GE_LOG(LogRender, Core::Verbosity::Error, "[OpenGL] OUT_OF_MEMORY");                 \
    case GL_STACK_OVERFLOW:                                                                \
      GE_LOG(LogRender, Core::Verbosity::Error, "[OpenGL] STACK_OVERFLOW");                \
    case GL_STACK_UNDERFLOW:                                                               \
      GE_LOG(LogRender, Core::Verbosity::Error, "[OpenGL] STACK_UNDERFLOW");               \
    }                                                                                      \
  }
