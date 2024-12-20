#include <Engine/Events/Input/InputEvents.h>
#include <Engine/Events/Renderer/EventResizeWindow.h>
#include <Engine/Events/ECS/ComponentAttached.h>
#include <Engine/Events/ECS/ComponentDetached.h>

GE_DEFINE_TYPE_METADATA(Engine::EventResizeWindow, Engine::TypeMetaData::Event)
GE_DEFINE_TYPE_METADATA(Engine::EventInput, Engine::TypeMetaData::Event)
GE_DEFINE_TYPE_METADATA(Engine::EventComponentAttached, Engine::TypeMetaData::Event)
GE_DEFINE_TYPE_METADATA(Engine::EventComponentDetached, Engine::TypeMetaData::Event)