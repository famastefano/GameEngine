#pragma once

#include <d3d11_4.h>
#include <dxgi1_6.h>

namespace Renderer::Private
{
using FactoryInterface          = IDXGIFactory7;
using AdapterInterface          = IDXGIAdapter4;
using OutputInterface           = IDXGIOutput6;
using DeviceInterface           = ID3D11Device5;
using ImmediateContextInterface    = ID3D11DeviceContext4;
using SwapChainInterface        = IDXGISwapChain4;
using RenderTargetViewInterface = ID3D11RenderTargetView;
using Texture2DInterface        = ID3D11Texture2D1;
using DepthStencilViewInterface = ID3D11DepthStencilView;
}