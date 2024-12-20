#pragma once

#include <Engine/Components/ComponentBase.h>
#include <Math/Vector.h>

namespace Engine::Components
{
class ENGINE_API TransformComponent : public ComponentBase
{
  GE_DECLARE_CLASS_TYPE_METADATA()

  Math::Vec3Df Position; // Z-component used for the
};
} // namespace Engine::Components
