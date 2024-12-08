#include <Engine/GameEngine/GameEngine.h>
#include <Engine/SubSystems/EngineSubSystem.h>
#include <Engine/SubSystems/SubSystemRegistration.h>

namespace Engine
{
static Core::Vector<SubSystemFactoryFn> SubSystemFactories[(i32)SubSystemType::Count] = {};

void RegisterSubSystemFactoryFn(SubSystemType const Type, SubSystemFactoryFn Fn)
{
  using enum SubSystemType;
  i32 const index = (i32)Type;
  checkf(u32(index) < (u32)SubSystemType::Count, "Invalid SubSystem Type.");
  SubSystemFactories[index].EmplaceBack(Fn);
}
GameEngine::GameEngine()
{
  auto const& factories = SubSystemFactories[(i32)SubSystemType::Engine];
  EngineSubSystems.Reserve(factories.Size());
  for (auto* factory : factories)
    EngineSubSystems.EmplaceBackUnsafe((EngineSubSystem*)factory());
}
GameEngine::~GameEngine()
{
  for (auto* subSystem : EngineSubSystems)
    delete subSystem;
}
void GameEngine::Tick(f32 DeltaTime)
{
  for (auto* subSystem : EngineSubSystems)
    subSystem->Tick(DeltaTime);
}
} // namespace Engine
