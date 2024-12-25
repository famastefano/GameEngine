#include <Engine/Components/SpriteComponent.h>
#include <Engine/Events/Renderer/EventResizeWindow.h>
#include <Engine/GameEngine/GameEngine.h>
#include <Engine/Interfaces/IEnvironment.h>
#include <Engine/SubSystems/ECS/EntityComponentSubSystem.h>
#include <Engine/SubSystems/Input/Base/Translator.h>
#include <Windows.h>
#include <chrono>
#include <hidusage.h>

struct Win32Env : Engine::IEnvironment
{
  Engine::GameEngine Engine_;
  RunningMode        Mode_;

  Win32Env(RunningMode Mode)
      : Mode_(Mode)
  {
  }

  Engine::GameEngine& GetGameEngine() override
  {
    return Engine_;
  }
  RunningMode GetRunningMode() override
  {
    return Mode_;
  }
};

Win32Env* gWin32Env{};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(_In_ HINSTANCE hInstance_, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
  (void)hInstance_;
  (void)hPrevInstance;
  (void)lpCmdLine;
  (void)nShowCmd;

  Win32Env env(Win32Env::RunningMode::Standalone);
  // ReSharper disable once CppDFALocalValueEscapesFunction
  gWin32Env = &env;

  env.NativeLoopCallback_ = (Win32Env::NativeLoopCallbackFn)WindowProc;

  env.Engine_.PreInitialize();
  env.Engine_.PostInitialize();

  auto* ECS = env.Engine_.FindSubSystem<Engine::EntityComponentSubSystem>();
  check(ECS);

  auto* actor = ECS->SpawnActor<Engine::Entities::ActorBase>("Debug Actor");
  check(actor);

  actor->AttachComponent<Engine::Components::SpriteComponent>();

  auto frameStart = std::chrono::steady_clock::now();

  while (true)
  {
    MSG msg{};
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if (msg.message == WM_QUIT)
        return (i32)msg.wParam;

      if (msg.message != WM_INPUT)
        TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    auto const                       frameEnd = std::chrono::steady_clock::now();
    std::chrono::duration<f32> const dt       = frameEnd - frameStart;
    env.Engine_.Tick(dt.count());
    frameStart = frameEnd;
  }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  Engine::NativeEvent const nev{
      .Windows_ = {
          .Hwnd_    = hwnd,
          .WParam_  = wParam,
          .LParam_  = lParam,
          .Message_ = uMsg,
      },
  };

  switch (uMsg)
  {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  case WM_SIZE: {
    Engine::EventResizeWindow ev;
    ev.Width_  = LOWORD(lParam);
    ev.Height_ = HIWORD(lParam);
    gWin32Env->Engine_.EnqueueEvent(ev);
    break;
  }

  case WM_INPUT: {
    Engine::EventInput ev;
    if (TranslateNativeEvent(nev, ev))
      gWin32Env->Engine_.EnqueueEvent(ev);
    break;
  }
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
