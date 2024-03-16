#include "Renderer/Engine.hpp"

#include "Core/AssertionMacros.hpp"
#include "Renderer/Sources/LogRenderer.hpp"

#include <wrl.h>

#pragma warning(disable : 6387) // assertion at the function entrance

template<typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

namespace Renderer
{
bool Engine::createDevice(Adapter& adapter) noexcept
{
    assert(!device && adapter.handle());

#if BUILD_CONFIG_DEBUG
    renderThreadId = GetCurrentThreadId();
#endif

#if BUILD_CONFIG_DEBUG
    constexpr UINT flags = D3D11_CREATE_DEVICE_DEBUG;
#else
    constexpr UINT flags = 0;
#endif

    constexpr D3D_FEATURE_LEVEL requestedLevels[2] = {D3D_FEATURE_LEVEL_11_1};
    ComPtr<ID3D11Device>        deviceV0;
    ComPtr<ID3D11DeviceContext> contextV0;

    HRESULT creationResult = D3D11CreateDevice(
        adapter.handle(),
        adapter.handle() ? D3D_DRIVER_TYPE_UNKNOWN : D3D_DRIVER_TYPE_HARDWARE,
        NULL,
        flags,
        requestedLevels,
        static_cast<UINT>(std::size(requestedLevels)),
        D3D11_SDK_VERSION,
        &deviceV0,
        NULL,
        &contextV0);

    ComPtr<Private::DeviceInterface>           _device;
    ComPtr<Private::ImmediateContextInterface> _context;
    if(FAILED(creationResult) || FAILED(deviceV0.As(&_device)) || FAILED(contextV0.As(&_context)))
        return false;

    device  = _device.Detach();
    context = _context.Detach();
    return true;
}

bool Engine::resizeSwapChain() noexcept
{
    if(swapChainRTV)
    {
        swapChainRTV->Release();
        swapChainRTV = nullptr;
    }

    if(depthStencilResource)
    {
        depthStencilResource->Release();
        depthStencilResource = nullptr;
    }

    if(depthStencilView)
    {
        depthStencilView->Release();
        depthStencilView = nullptr;
    }

    DXGI_SWAP_CHAIN_DESC1 scDesc;
    swapChain->GetDesc1(&scDesc);

    if(FAILED(swapChain->ResizeBuffers(scDesc.BufferCount, 0, 0, DXGI_FORMAT_UNKNOWN, scDesc.Flags)))
        return false;

    swapChain->GetDesc1(&scDesc);

    ComPtr<ID3D11Texture2D> backBuffer;
    if(FAILED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer))) ||
       FAILED(device->CreateRenderTargetView(backBuffer.Get(), 0, &swapChainRTV)))
        return false;

    D3D11_TEXTURE2D_DESC1 depthStencilDesc{};
    depthStencilDesc.Width          = scDesc.Width;
    depthStencilDesc.Height         = scDesc.Height;
    depthStencilDesc.MipLevels      = 1;
    depthStencilDesc.ArraySize      = 1;
    depthStencilDesc.Format         = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc     = scDesc.SampleDesc;
    depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags      = 0;
    depthStencilDesc.TextureLayout  = D3D11_TEXTURE_LAYOUT_UNDEFINED;

    ComPtr<ID3D11Texture2D1>       dsRes;
    ComPtr<ID3D11DepthStencilView> dsView;
    if(FAILED(device->CreateTexture2D1(&depthStencilDesc, NULL, dsRes.GetAddressOf())) ||
       FAILED(device->CreateDepthStencilView(dsRes.Get(), 0, dsView.GetAddressOf())))
        return false;

    context->OMSetRenderTargets(1, &swapChainRTV, dsView.Get());

    D3D11_VIEWPORT vp{};
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width    = static_cast<float>(scDesc.Width);
    vp.Height   = static_cast<float>(scDesc.Height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    context->RSSetViewports(1, &vp);

    depthStencilResource = dsRes.Detach();
    depthStencilView     = dsView.Detach();
    return true;
}

void Engine::destroySwapChain()
{
    // https://learn.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-id3d11devicecontext-flush#remarks
    context->ClearState();
    swapChain->Release();
    swapChainRTV->Release();
    depthStencilResource->Release();
    depthStencilView->Release();
    context->Flush();

    swapChain            = nullptr;
    swapChainRTV         = nullptr;
    depthStencilResource = nullptr;
    depthStencilView     = nullptr;
}

Engine::Engine() noexcept
{
#ifdef BUILD_CONFIG_DEBUG
    constexpr UINT factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#else
    constexpr UINT factoryFlags = 0;
#endif

    ComPtr<IDXGIFactory2> factory2;
    HRESULT               hr = CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&factory2));
    assertf(hr == S_OK, L"Failed to create the IDXGIFactory2 object.");
    if(SUCCEEDED(hr))
        hr = factory2->QueryInterface(&factory);
    assertf(hr == S_OK, L"Failed to obtain the latest IDXGIFactory interface version.");
}

Engine::~Engine()
{
    if(swapChain)
        destroySwapChain();

    if(context)
    {
        context->ClearState();
        context->Release();
    }

    if(device)
        device->Release();

    if(factory)
        factory->Release();
}

std::vector<Adapter> Engine::adapters() const noexcept
{
    std::vector<Adapter>       adapters;
    Private::AdapterInterface* currAdapter{};
    DXGI_ADAPTER_DESC3         desc{};

    for(int i = 0;
        factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&currAdapter)) != DXGI_ERROR_NOT_FOUND;
        ++i)
    {
        currAdapter->GetDesc3(&desc);

        Vendor vendor;
        switch(desc.VendorId)
        {
        case 0x10DE:
            vendor = Vendor::NVIDIA;
            break;
        case 0x1002:
            vendor = Vendor::AMD;
            break;
        case 0x8086:
            vendor = Vendor::Intel;
            break;
        default:
            vendor = Vendor::Unknown;
            break;
        }
        adapters.emplace_back(currAdapter, desc.Description, vendor, desc.DedicatedVideoMemory, desc.DedicatedSystemMemory, desc.SharedSystemMemory);
    }

    return adapters;
}

std::vector<Output> Engine::outputs(Adapter& adapter) const noexcept
{
    assert(adapter.handle() != nullptr);

    std::vector<Output> outputs;
    IDXGIOutput*        currOutput{};
    DXGI_OUTPUT_DESC1   desc{};
    for(int i = 0;
        adapter.handle()->EnumOutputs(i, &currOutput) != DXGI_ERROR_NOT_FOUND;
        ++i)
    {
        Private::OutputInterface* outputV6{};
        if(S_OK == currOutput->QueryInterface(&outputV6))
        {
            outputV6->GetDesc1(&desc);
            outputs.emplace_back(outputV6, desc.DeviceName);
        }
    }
    return outputs;
}

bool Engine::init(Adapter& adapter, Output& output, SwapChainOptions const& options) noexcept
{
    // TODO: destroy offscreen render target if any

    if(swapChain)
        destroySwapChain();

    if(!createDevice(adapter))
        return false;

    syncInterval = options.syncInterval;

    assert(output.handle() != nullptr);
    assert(options.windowHandle != nullptr);

    // https://learn.microsoft.com/en-us/windows/win32/api/dxgi1_2/ns-dxgi1_2-dxgi_swap_chain_desc1?devlangs=cpp&f1url=%3FappId%3DDev16IDEF1%26l%3DEN-US%26k%3Dk(DXGI1_2%252FDXGI_SWAP_CHAIN_DESC1)%3Bk(DXGI_SWAP_CHAIN_DESC1)%3Bk(DevLang-C%252B%252B)%3Bk(TargetOS-Windows)%26rd%3Dtrue#remarks
    assertf(options.backBufferCount >= 2 && options.backBufferCount <= 16, L"To create a swap chain, the number of back buffers shall be between 2 and 16.");
    assert(options.format == OutputFormat::RGBA8_UNORM || options.format == OutputFormat::RGBA16_FLOAT);
    assertf(options.width > 0 && options.height > 0, L"The minimum size to create a swap chain is 1x1.");

    UINT swapChainFlags = 0;

    BOOL allowTearing = false;
    if(SUCCEEDED(factory->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing))) && allowTearing)
    {
        swapChainFlags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
        presentFlags = DXGI_PRESENT_ALLOW_TEARING;

        // See https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-present regarding DXGI_PRESENT_ALLOW_TEARING
        factory->MakeWindowAssociation(static_cast<HWND>(options.windowHandle), DXGI_MWA_NO_ALT_ENTER);
    }

    DXGI_MODE_DESC1 preferredMode, closestMode{};
    preferredMode.Width            = options.width;
    preferredMode.Height           = options.height;
    preferredMode.RefreshRate      = DXGI_RATIONAL{1, options.refreshRate};
    preferredMode.Format           = options.format == OutputFormat::RGBA8_UNORM ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R16G16B16A16_FLOAT;
    preferredMode.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    preferredMode.Scaling          = DXGI_MODE_SCALING_UNSPECIFIED;
    preferredMode.Stereo           = FALSE;
    if(!SUCCEEDED(output.handle()->FindClosestMatchingMode1(&preferredMode, &closestMode, device)))
        closestMode = preferredMode; // Let's hope for the best

    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.Width              = closestMode.Width;
    swapChainDesc.Height             = closestMode.Height;
    swapChainDesc.Format             = closestMode.Format;
    swapChainDesc.SampleDesc.Count   = 1;
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount        = options.backBufferCount;
    swapChainDesc.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;
    swapChainDesc.Flags              = swapChainFlags;

    switch(preferredMode.Scaling)
    {
    default:
    case DXGI_MODE_SCALING_UNSPECIFIED:
        swapChainDesc.Scaling = DXGI_SCALING_NONE;
        break;
    case DXGI_MODE_SCALING_CENTERED:
        swapChainDesc.Scaling = DXGI_SCALING_ASPECT_RATIO_STRETCH;
        break;
    case DXGI_MODE_SCALING_STRETCHED:
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        break;
    }

    ComPtr<IDXGISwapChain1> swapChainV1;
    HRESULT                 creationResult = E_FAIL;
    presentFlags                           = 0;

    if(options.presentMode == PresentMode::Windowed)
    {
        creationResult = factory->CreateSwapChainForHwnd(device, static_cast<HWND>(options.windowHandle), &swapChainDesc, NULL, NULL, swapChainV1.GetAddressOf());
    }
    else
    {
        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc{};
        fullscreenDesc.RefreshRate      = closestMode.RefreshRate;
        fullscreenDesc.ScanlineOrdering = closestMode.ScanlineOrdering;
        fullscreenDesc.Scaling          = closestMode.Scaling;
        fullscreenDesc.Windowed         = options.presentMode == PresentMode::WindowedFullscreen;

        creationResult = factory->CreateSwapChainForHwnd(device, static_cast<HWND>(options.windowHandle), &swapChainDesc, &fullscreenDesc, NULL, swapChainV1.GetAddressOf());
    }

    ComPtr<Private::SwapChainInterface> _swapChain;
    if(FAILED(creationResult) || FAILED(swapChainV1.As(&_swapChain)))
        return false;

    swapChain = _swapChain.Detach();

    if(!resizeSwapChain())
        return false;

    return true;
}

bool Engine::resizeOutput(unsigned int width, unsigned int height) noexcept
{
    if(context)
    {
        context->OMSetRenderTargets(0, NULL, NULL);
        (void)width;
        (void)height;
        if(swapChain)
            return resizeSwapChain(); // swap chain gets the new size directly from the client area
        // TODO: resize offscreen render target
        return false;
    }
    return true;
}

void Engine::tick() noexcept
{
#if BUILD_CONFIG_DEBUG
    assertf(renderThreadId == GetCurrentThreadId(), L"Renderer::Engine::tick() can only be called from the thread that initialized the engine (ID: {}), but thread with ID {} did instead.", renderThreadId, GetCurrentThreadId());
#endif

    constexpr float black[4]{};
    context->ClearRenderTargetView(swapChainRTV, black);
    context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    if(swapChain)
    {
        HRESULT r = swapChain->Present(static_cast<UINT>(syncInterval), presentFlags);
        if(SUCCEEDED(r))
            return;
    }
}
}

#pragma warning(default : 6387) // assertion at the function entrance
