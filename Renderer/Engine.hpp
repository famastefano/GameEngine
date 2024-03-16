#pragma once

#include "Core/BuildConfig.hpp"
#include "Renderer/Adapter.hpp"
#include "Renderer/Output.hpp"
#include "Renderer/RHI.hpp"

#include <vector>

namespace Renderer
{
class Engine
{
#if BUILD_CONFIG_DEBUG
    unsigned long long renderThreadId{};
#endif

    Private::FactoryInterface*          factory{};
    Private::DeviceInterface*           device{};
    Private::ImmediateContextInterface* context{};
    Private::SwapChainInterface*        swapChain{};
    Private::RenderTargetViewInterface* swapChainRTV{};
    Private::Texture2DInterface*        depthStencilResource{};
    Private::DepthStencilViewInterface* depthStencilView{};

    UINT presentFlags{};

    Private::DeviceInterface* createDevice(Adapter& adapter) noexcept;

    bool resizeSwapChain() noexcept;

    void destroySwapChain();

    public:
    enum class SyncInterval : int
    {
        VariableRefreshRate = 0,
        EveryVBlank         = 1,
        EveryHalfVBlank     = 2,
    };

    enum class PresentMode
    {
        Windowed,
        WindowedFullscreen,
        ExclusiveFullscreen,
    };

    enum class OutputFormat
    {
        RGBA8_UNORM,
        RGBA16_FLOAT,
    };

    struct SwapChainOptions
    {
        void*        windowHandle{};
        unsigned int width  = 1;
        unsigned int height = 1;
        unsigned int refreshRate{};       // Output Hz
        unsigned int backBufferCount = 2; // Minimum supported mode: Double Buffering
        SyncInterval syncInterval{};
        PresentMode  presentMode{};
        OutputFormat format = OutputFormat::RGBA8_UNORM;
    };

    Engine() noexcept;
    Engine(Engine const&)            = delete;
    Engine(Engine&&)                 = delete;
    Engine& operator=(Engine const&) = delete;
    Engine& operator=(Engine&&)      = delete;
    ~Engine();

    std::vector<Adapter> adapters() const noexcept;
    std::vector<Output>  outputs(Adapter& adapter) const noexcept;

    bool init(Adapter& adapter, Output& output, SwapChainOptions const& options) noexcept;

    bool resizeOutput(unsigned int width, unsigned int height) noexcept;
};
}
