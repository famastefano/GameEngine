#include <Engine/GameEngine/GameEngine.h>
#include <Engine/LogEngine.h>
#include <Engine/SubSystems/EngineSubSystem.h>

GE_DEFINE_TYPE_METADATA(Engine::GameEngine, Engine::TypeMetaData::Engine)

namespace Engine
{
GameEngine::GameEngine()
{
  Core::Vector<TypeMetaData const*> subSystemsMetaData;
  for (auto const& [ID, MetaData] : GetTypesMetaData())
  {
    if (MetaData->Kind_ == TypeMetaData::EngineSubSystem)
      subSystemsMetaData.EmplaceBack(MetaData);
  }
  EngineSubSystems_.Reserve(subSystemsMetaData.Size());
  for (auto const* metaData : subSystemsMetaData)
  {
    auto* subSystem = (EngineSubSystem*)metaData->Factory_();
    EngineSubSystems_.EmplaceBackUnsafe(subSystem);
  }
}
GameEngine::~GameEngine()
{
  for (auto* subSystem : EngineSubSystems_)
    delete subSystem;
}
void GameEngine::PreInitialize()
{
  GE_LOG(LogEngine, Core::Verbosity::Info, "Calling EngineSubSystem::PreInitialize");
  for (auto* subSystem : EngineSubSystems_)
    subSystem->PreInitialize();
}
void GameEngine::PostInitialize()
{
  GE_LOG(LogEngine, Core::Verbosity::Info, "Calling EngineSubSystem::PostInitialize");
  for (auto* subSystem : EngineSubSystems_)
    subSystem->PostInitialize();
}
void GameEngine::Tick(f32 DeltaTime)
{
  GE_LOG(LogEngine, Core::Verbosity::Debug, "Calling EngineSubSystem::Tick - %.4fs", DeltaTime);
  for (auto* subSystem : EngineSubSystems_)
    subSystem->Tick(DeltaTime);
}
void GameEngine::EnqueueEvent(EventBase& Event)
{
  for (auto* subSystem : EngineSubSystems_)
    if (subSystem->HandleEvent(Event))
      return;
}
} // namespace Engine
