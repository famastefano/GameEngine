#include <Engine/GameEngine/GameEngine.h>
#include <Engine/Private/LogEngine.h>
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
void GameEngine::PreInitialize()
{
  GE_LOG(LogEngine, Core::Verbosity::Info, "Calling EngineSubSystem::PreInitialize");
  for (auto* subSystem : EngineSubSystems)
    subSystem->PreInitialize();
}
void GameEngine::PostInitialize()
{
  GE_LOG(LogEngine, Core::Verbosity::Info, "Calling EngineSubSystem::PostInitialize");
  for (auto* subSystem : EngineSubSystems)
    subSystem->PostInitialize();
}
void GameEngine::Tick(f32 DeltaTime)
{
  GE_LOG(LogEngine, Core::Verbosity::Debug, "Calling EngineSubSystem::Tick");
  for (auto* subSystem : EngineSubSystems)
    subSystem->Tick(DeltaTime);
}
} // namespace Engine