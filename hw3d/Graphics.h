#pragma once
#include "Win.h"
#include <d3d11.h>

class Graphics
{
public:
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept
	{
		const float color[] = { r, g, b, 1.0f };
		pContext->ClearRenderTargetView(pTarget, color);
	}

private:
	//a pointer to our device to be filled by the
	//call to D3D11CreateDeviceAndSwapChain
	ID3D11Device* pDevice = nullptr;
	//pointer to the swap chain
	//(the series of framebuffers)
	IDXGISwapChain* pSwap = nullptr;
	//pointer to our device context 
	//(what we use to do graphics shit)
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
};