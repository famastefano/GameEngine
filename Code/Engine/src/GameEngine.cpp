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
  EngineSubSystems_.Reserve(factories.Size());
  for (auto* factory : factories)
  {
    auto* subSystem = (EngineSubSystem*)factory();
    const bool added = EngineSubSystems_.TryEmplace(subSystem->Name().CalculateHash(), subSystem);
    checkf(added, "SubSystem %.*s already added.", subSystem->Name().Size(), subSystem->Name().Data());
  }
}
GameEngine::~GameEngine()
{
  for (auto& [key, subsystem] : EngineSubSystems_)
    delete subsystem;
}
void GameEngine::PreInitialize()
{
  GE_LOG(LogEngine, Core::Verbosity::Info, "Calling EngineSubSystem::PreInitialize");
  for (const auto& [key, subSystem] : EngineSubSystems_)
    subSystem->PreInitialize();
}
void GameEngine::PostInitialize()
{
  GE_LOG(LogEngine, Core::Verbosity::Info, "Calling EngineSubSystem::PostInitialize");
  for (const auto& [key, subSystem] : EngineSubSystems_)
    subSystem->PostInitialize();
}
void GameEngine::Tick(f32 DeltaTime)
{
  GE_LOG(LogEngine, Core::Verbosity::Debug, "Calling EngineSubSystem::Tick - %.4fs", DeltaTime);
  for (const auto& [key, subSystem] : EngineSubSystems_)
    subSystem->Tick(DeltaTime);
}
EngineSubSystem* GameEngine::GetEngineSubSystem(Core::StringView<char> Name)
{
  auto subSystem = EngineSubSystems_.Find(Name.CalculateHash());
  return subSystem ? *subSystem : nullptr;
}
void GameEngine::EnqueueEvent(EventBase& Event)
{
  for (auto& [key, subSystem] : EngineSubSystems_)
    if (subSystem->HandleEvent(Event))
      return;
}
} // namespace Engine
