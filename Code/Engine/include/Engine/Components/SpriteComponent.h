#pragma once

#include <Engine/Components/ComponentBase.h>

namespace Engine::Components
{
class ENGINE_API SpriteComponent : public ComponentBase
{
  GE_DECLARE_CLASS_TYPE_METADATA()

public:
  SpriteComponent();
  ~SpriteComponent() override;
};
} // namespace Engine::Components
