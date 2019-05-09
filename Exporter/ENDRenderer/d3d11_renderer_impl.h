#pragma once

#include "renderer.h"
#include "view.h"

#include <fstream>
#include <vector>
#include <iostream>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <dxgi1_2.h>
#include <d3d11_2.h>
#include <DirectXColors.h>
#include <fstream>
#include <windowsx.h>

#include "DDSTextureLoader.h"
#include "pools.h"
#include "debug_renderer.h"
#include "frustum_culling.h"
#include "WICTextureLoader.h"
#include "XTime.h"
#include "Structs.h"


#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DXGI.lib")

namespace end
{
	struct renderer::impl
	{
		ID3D11Device *device = nullptr;
		ID3D11DeviceContext *context = nullptr;
		IDXGISwapChain *swapchain = nullptr;

		ID3D11RenderTargetView*		render_target[VIEW_RENDER_TARGET::COUNT]{};

		ID3D11DepthStencilView*		depthStencilView[VIEW_DEPTH_STENCIL::COUNT]{};

		ID3D11DepthStencilState*	depthStencilState[STATE_DEPTH_STENCIL::COUNT]{};

		ID3D11Buffer*				vertex_buffer[VERTEX_BUFFER::COUNT]{};

		ID3D11Buffer*				index_buffer[INDEX_BUFFER::COUNT]{};

		ID3D11InputLayout*			input_layout[INPUT_LAYOUT::COUNT]{};

		ID3D11VertexShader*			vertex_shader[VERTEX_SHADER::COUNT]{};

		ID3D11Buffer*				constant_buffer[CONSTANT_BUFFER::COUNT]{};

		ID3D11PixelShader*			pixel_shader[PIXEL_SHADER::COUNT]{};

		ID3D11RasterizerState*		rasterState[STATE_RASTERIZER::COUNT]{};

		D3D11_VIEWPORT				view_port[VIEWPORT::COUNT]{};

		ID3D11ShaderResourceView*	resourceView[SHADER_RESOURCE_VIEW::COUNT]{};

		ID3D11SamplerState*			samplerState[SAMPLER_STATE::COUNT]{};

		ID3D11Texture2D*			texture[TEXTURE::COUNT]{};

		Skinned_mesh skinnedMesh;
		Simple_mesh simpleMesh;

		XTime timer;
		XTime animTimer;

		int animStep = 0;
		size_t maxFrames = 0;

		XMMATRIX* qs = nullptr;
		size_t qSize = 0;
		XMMATRIX matrices[JOINTCOUNT];

		AnimClip animation;


		D3D11_MAPPED_SUBRESOURCE rs = {};
		view_t cb;

		int rasterStatenum = 0;

		XMMATRIX GridWM = XMMatrixIdentity();

		impl(native_handle_type window_handle, view_t& default_view)
		{

#pragma region MVP

			XMMATRIX mat = XMMatrixTranslation(0, 0, 3);
			default_view.world = mat;
			XMVECTOR eye = { 0.0f, 15, -15 };
			XMVECTOR at = { 0.0f, 0, 0.0f };
			XMVECTOR up{ 0,1,0 };
			mat = XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up));
			default_view.view = mat;
			mat = XMMatrixPerspectiveFovLH(XMConvertToRadians(65), 1280 / 720.0f, 0.1f, 100000.0f);
			default_view.proj = mat;
#pragma endregion

#pragma region Device&Swapchain
			RECT windowrect = {};
			GetWindowRect((HWND)window_handle, &windowrect);
			UINT width = windowrect.right - windowrect.left;
			UINT height = windowrect.bottom - windowrect.top;

			DXGI_SWAP_CHAIN_DESC sd;
			//ZeroMemory(&sd, sizeof(sd));
			//sd.Windowed = true;
			sd.BufferCount = 1;
			sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			sd.BufferDesc.Width = width;
			sd.BufferDesc.Height = height;
			sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			sd.SampleDesc.Count = 2;
			sd.SampleDesc.Quality = 0;
			sd.OutputWindow = (HWND)window_handle;

			D3D_FEATURE_LEVEL  FeatureLevelsRequested = D3D_FEATURE_LEVEL_11_0;
			UINT               numLevelsRequested = 1;
			D3D_FEATURE_LEVEL  FeatureLevelsSupported;

			HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL,
				D3D_DRIVER_TYPE_HARDWARE,
				NULL,
				D3D11_CREATE_DEVICE_DEBUG,
				&FeatureLevelsRequested,
				numLevelsRequested,
				D3D11_SDK_VERSION,
				&sd,
				&swapchain,
				&device,
				&FeatureLevelsSupported,
				&context);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"device and swapchain creation failed", L"Error", MB_OK);
				exit(0);
			}

#pragma endregion

#pragma region ViewPort
			// creating viewport
			CD3D11_VIEWPORT vp = {};
			ZeroMemory(&vp, sizeof(vp));
			DXGI_SWAP_CHAIN_DESC sdesc = {};
			swapchain->GetDesc(&sdesc);
			vp.Height = (float)sdesc.BufferDesc.Height;
			vp.Width = (float)sdesc.BufferDesc.Width;
			vp.MaxDepth = 1;


			view_port[VIEWPORT::DEFAULT] = vp;
#pragma endregion

#pragma region RTV

			// Create render target view
			ID3D11Resource* pBB;
			hr = swapchain->GetBuffer(0, __uuidof(pBB), reinterpret_cast<void**>(&pBB));
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"render target resource failed", L"Error", MB_OK);
				exit(0);
			}
			hr = device->CreateRenderTargetView(pBB, NULL, &render_target[VIEW_RENDER_TARGET::DEFAULT]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"render target creation failed", L"Error", MB_OK);
				exit(0);
			}
			pBB->Release();

#pragma endregion

#pragma region DepthStencilState
			D3D11_DEPTH_STENCIL_DESC dsDesc;

			// Depth test parameters
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

			// Stencil test parameters
			dsDesc.StencilEnable = true;
			dsDesc.StencilReadMask = 0xFF;
			dsDesc.StencilWriteMask = 0xFF;

			// Stencil operations if pixel is front-facing
			dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// Stencil operations if pixel is back-facing
			dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// creating depth stencil state
			hr = device->CreateDepthStencilState(&dsDesc, &depthStencilState[STATE_DEPTH_STENCIL::DEFAULT]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"depth stencil state creation failed", L"Error", MB_OK);
				exit(0);
			}
#pragma endregion

#pragma region DepthStencilView

			ID3D11Texture2D* pDepthStencil = NULL;
			D3D11_TEXTURE2D_DESC descDepth = {};
			descDepth.Width = sd.BufferDesc.Width;
			descDepth.Height = sd.BufferDesc.Height;
			descDepth.MipLevels = 1;
			descDepth.ArraySize = 1;
			descDepth.Format = DXGI_FORMAT_D32_FLOAT;
			descDepth.SampleDesc.Count = sd.SampleDesc.Count;
			descDepth.SampleDesc.Quality = sd.SampleDesc.Quality;
			descDepth.Usage = D3D11_USAGE_DEFAULT;
			descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			descDepth.CPUAccessFlags = 0;
			descDepth.MiscFlags = 0;

			hr = device->CreateTexture2D(&descDepth, NULL, &pDepthStencil);

			D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
			descDSV.Format = DXGI_FORMAT_D32_FLOAT;
			descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			descDSV.Texture2D.MipSlice = 0;

			// creating depth stencil view
			hr = device->CreateDepthStencilView(pDepthStencil, &descDSV, &depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"depth stencil view creation failed", L"Error", MB_OK);
				exit(0);
			}
			pDepthStencil->Release();

#pragma endregion

#pragma region RasterizerState
			D3D11_RASTERIZER_DESC rdsc = {};
			rdsc.FillMode = D3D11_FILL_SOLID;
			rdsc.CullMode = D3D11_CULL_BACK;
			//rdsc.AntialiasedLineEnable = true;
			//rdsc.MultisampleEnable = true;

			// creating rasterizer state
			hr = device->CreateRasterizerState(&rdsc, &rasterState[STATE_RASTERIZER::DEFAULT]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"raster state creation failed", L"Error", MB_OK);
				exit(0);
			}
			context->RSSetState(rasterState[STATE_RASTERIZER::DEFAULT]);

			ZeroMemory(&rdsc, sizeof(rdsc));
			rdsc.FillMode = D3D11_FILL_SOLID;
			rdsc.CullMode = D3D11_CULL_FRONT;
			// creating rasterizer state
			hr = device->CreateRasterizerState(&rdsc, &rasterState[STATE_RASTERIZER::CULLFRONT]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"raster state creation failed", L"Error", MB_OK);
				exit(0);
			}

			ZeroMemory(&rdsc, sizeof(rdsc));
			rdsc.FillMode = D3D11_FILL_SOLID;
			rdsc.CullMode = D3D11_CULL_NONE;
			// creating rasterizer state
			hr = device->CreateRasterizerState(&rdsc, &rasterState[STATE_RASTERIZER::NONE]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"raster state creation failed", L"Error", MB_OK);
				exit(0);
			}

			ZeroMemory(&rdsc, sizeof(rdsc));
			rdsc.FillMode = D3D11_FILL_WIREFRAME;
			rdsc.CullMode = D3D11_CULL_BACK;
			// creating rasterizer state
			hr = device->CreateRasterizerState(&rdsc, &rasterState[STATE_RASTERIZER::WIREFRAME]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"raster state creation failed", L"Error", MB_OK);
				exit(0);
			}

#pragma endregion

#pragma region ConstantBuffers
			// creating vs constant buffer
			D3D11_BUFFER_DESC bd = {};
			bd.ByteWidth = sizeof(view_t);
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;


			hr = device->CreateBuffer(&bd, nullptr, &constant_buffer[CONSTANT_BUFFER::MVP]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"constant buffer creation failed", L"Error", MB_OK);
				exit(0);
			}

			// ps constant buffer
			ZeroMemory(&bd, sizeof(bd));
			bd.ByteWidth = sizeof(psCB);
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

			hr = device->CreateBuffer(&bd, nullptr, &constant_buffer[CONSTANT_BUFFER::PIXEL]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"pixel constant buffer creation failed", L"Error", MB_OK);
				exit(0);
			}
#pragma endregion

#pragma region Shaders&InputL

			// loading vertex and pixel shaders
			std::ifstream vsFile("vs_cube.cso", std::ios::binary);				std::vector<char> vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
			std::ifstream psFile("ps_cube.cso", std::ios::binary);				std::vector<char> psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
			std::ifstream vsfile("VertexShader.cso", std::ios::binary);			std::vector<char> vData = { std::istreambuf_iterator<char>(vsfile), std::istreambuf_iterator<char>() };
			std::ifstream psfile("PixelShader.cso", std::ios::binary);			std::vector<char> pData = { std::istreambuf_iterator<char>(psfile), std::istreambuf_iterator<char>() };
			std::ifstream simplevs("SimpleVertexShader.cso", std::ios::binary); std::vector<char> simplevsData = { std::istreambuf_iterator<char>(simplevs), std::istreambuf_iterator<char>() };
			std::ifstream simpleps("SimplePixelShader.cso", std::ios::binary);  std::vector<char> simplepsData = { std::istreambuf_iterator<char>(simpleps), std::istreambuf_iterator<char>() };

			// Creating Vertex and pixel shaders

			// ENDERENDERER CUBE VS & PS
			hr = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &vertex_shader[VERTEX_SHADER::ENDRENDERER]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"Vertex shader creation failed", L"Error", MB_OK);
				exit(0);
			}
			hr = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &pixel_shader[PIXEL_SHADER::ENDRENDERER]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"Pixel shader creation failed", L"Error", MB_OK);
				exit(0);
			}

			// COLORED VERTEX VS & PS
			hr = device->CreateVertexShader(vData.data(), vData.size(), nullptr, &vertex_shader[VERTEX_SHADER::COLORED_VERTEX]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"Vertex shader creation failed", L"Error", MB_OK);
				exit(0);
			}
			hr = device->CreatePixelShader(pData.data(), pData.size(), nullptr, &pixel_shader[PIXEL_SHADER::COLORED_VERTEX]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"Pixel shader creation failed", L"Error", MB_OK);
				exit(0);
			}

			// MAGE VS & PS
			hr = device->CreateVertexShader(simplevsData.data(), simplevsData.size(), nullptr, &vertex_shader[VERTEX_SHADER::MAGE]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"Simple Vertex shader creation failed", L"Error", MB_OK);
				exit(0);
			}
			hr = device->CreatePixelShader(simplepsData.data(), simplepsData.size(), nullptr, &pixel_shader[PIXEL_SHADER::MAGE]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"Simple Pixel shader creation failed", L"Error", MB_OK);
				exit(0);
			}

			// Creating Input layouts

			// ENDRENDERER Cube layout
			D3D11_INPUT_ELEMENT_DESC layout[] = {
				"SV_VertexID", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };

			hr = device->CreateInputLayout(layout, _ARRAYSIZE(layout), vsData.data(), vsData.size(), &input_layout[INPUT_LAYOUT::ENDRENDERER]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"input layout creation failed", L"Error", MB_OK);
				exit(0);
			}

			// COLORED VERTEX layout
			D3D11_INPUT_ELEMENT_DESC gridLayout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
			};
			hr = device->CreateInputLayout(gridLayout, _ARRAYSIZE(gridLayout), vData.data(), vData.size(), &input_layout[INPUT_LAYOUT::COLORED_VERTEX]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"input layout of colored vertex creation failed", L"Error", MB_OK);
				exit(0);
			}

			// MAGE layout
			D3D11_INPUT_ELEMENT_DESC fbxlayout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "INDICES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
				//{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			};
			hr = device->CreateInputLayout(fbxlayout, _ARRAYSIZE(fbxlayout), simplevsData.data(), simplevsData.size(), &input_layout[INPUT_LAYOUT::MAGE]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"Input layout of mage creation failed", L"Error", MB_OK);
				exit(0);
			}

#pragma endregion

#pragma region Model
			          
			// Creating vertex buffer for debug lines
			ZeroMemory(&bd, sizeof(bd));
			bd.ByteWidth = sizeof(colored_vertex) * (UINT)debug_renderer::get_line_vert_capacity();
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bd.Usage = D3D11_USAGE_DYNAMIC;

			hr = device->CreateBuffer(&bd, nullptr, &vertex_buffer[VERTEX_BUFFER::COLORED_VERTEX]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"v buffer creation failed", L"Error", MB_OK);
				exit(0);
			}

			const wchar_t* diffusePath  = L"..//MageAssets//battleMage.fbm//PPG_3D_Player_D.png";
			const wchar_t* specularPath = L"..//MageAssets//battleMage.fbm//PPG_3D_Player_spec.png";
			const wchar_t* emissivePath = L"..//MageAssets//battleMage.fbm//PPG_3D_Player_emissive.png";
			const wchar_t* NormalPath	  = L"..//MageAssets//battleMage.fbm//PPG_3D_Player_N.png";

			//const wchar_t* diffusePath  = L"..//MageAssets//Diffuse.png";
			//const wchar_t* specularPath = L"..//MageAssets//Specular.png";
			//const wchar_t* emissivePath = nullptr;
			//const wchar_t* NormalPath	= L"..//MageAssets//Normal.png";

			//const wchar_t* diffusePath = L"..//MageAssets//Enemy_Archer_Diffuse.png";
			//const wchar_t* specularPath = L"..//MageAssets//Enemy_Archer_Specular.png";
			//const wchar_t* emissivePath = nullptr;
			//const wchar_t* NormalPath = nullptr;

			//const wchar_t* diffusePath = L"..//MageAssets//UV_Base_Color.png";
			//const wchar_t* specularPath = nullptr;
			//const wchar_t* emissivePath = nullptr;

			//const wchar_t* diffusePath = L"..//KnightAssets//paladin_D.png";
			//const wchar_t* specularPath = nullptr;
			//const wchar_t* emissivePath = nullptr;
			//const wchar_t* NormalPath = nullptr; 

			//const wchar_t* diffusePath = L"..//DemonAssets//MAW_diffuse.png";
			//const wchar_t* specularPath = nullptr;
			//const wchar_t* emissivePath = nullptr;
			//const wchar_t* NormalPath = nullptr;

			const char* SkinnedMeshPath = "..//MageAssets//Mage.bin";
			const char* BindPosePath	= "..//MageAssets//MageBind.bin";
			const char* AnimationPath	= "..//MageAssets//MageHurt.bin";

			// Skinned mesh
			std::ifstream file(SkinnedMeshPath, std::ios::ios_base::binary);
			if (file.is_open())
			{
				file.read((char *)&skinnedMesh.vert_count, sizeof(int));
				skinnedMesh.verts = new Skinned_vert[skinnedMesh.vert_count];
				file.read((char *)skinnedMesh.verts, sizeof(Skinned_vert) * skinnedMesh.vert_count);

				file.read((char *)&skinnedMesh.index_count, sizeof(skinnedMesh.index_count));
				skinnedMesh.indices = new uint32_t[skinnedMesh.index_count];
				file.read((char *)skinnedMesh.indices, sizeof(int) * skinnedMesh.index_count);
			}
			else
			{
				MessageBox(nullptr, L"No Mesh file", L"Error", MB_OK);
				exit(0);
				file.close();
			}
			file.close();

			// Simple mesh
			/*file.open("../mageassets/ArcherBind.bin", std::ios::ios_base::binary);
			if (file.is_open())
			{
				file.read((char *)&simpleMesh.vert_count, sizeof(int));
				simpleMesh.verts = new Simple_vert[simpleMesh.vert_count];
				file.read((char *)simpleMesh.verts, sizeof(Simple_vert) * simpleMesh.vert_count);

				file.read((char *)&simpleMesh.index_count, sizeof(simpleMesh.index_count));
				simpleMesh.indices = new uint32_t[simpleMesh.index_count];
				file.read((char *)simpleMesh.indices, sizeof(int) * simpleMesh.index_count);
			}
			else
			{
				MessageBox(nullptr, L"No mesh file", L"Error", MB_OK);
				exit(0);
				file.close();
			}
			file.close();*/

			// Bind pose 
			file.open(BindPosePath, std::ios::ios_base::binary);
			if (file.is_open())
			{

				file.read((char*)&qSize, sizeof(size_t));
				qs = new XMMATRIX[qSize];
				file.read((char*)qs, sizeof(XMMATRIX) * qSize);

			}
			else
			{
				MessageBox(nullptr, L"No Bind file", L"Error", MB_OK);
				exit(0);
				file.close();
			}
			file.close();

			// Animations
			file.open(AnimationPath, std::ios::ios_base::binary);
			if (file.is_open())
			{
				file.read((char*)&animation.duration, sizeof(double));
				file.read((char*)&maxFrames, sizeof(size_t));
				animation.frames = new KeyFrame[maxFrames];
				for (size_t i = 0; i < maxFrames; i++)
				{
					animation.frames[i].joints = new Quarternions[qSize];
					file.read((char*)animation.frames[i].joints, sizeof(Quarternions) * qSize);
					file.read((char*)&animation.frames[i].time, sizeof(double));
				}
			}
			else
			{
				MessageBox(nullptr, L"No Animation file", L"Error", MB_OK);
				exit(0);
				file.close();
			}
			file.close();

			// BattleMage vertex buffer
			ZeroMemory(&bd, sizeof(bd));
			bd.ByteWidth = sizeof(Skinned_vert) * skinnedMesh.vert_count;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bd.Usage = D3D11_USAGE_DYNAMIC;

			D3D11_SUBRESOURCE_DATA sdd = {};
			sdd.pSysMem = skinnedMesh.verts;

			hr = device->CreateBuffer(&bd, &sdd, &vertex_buffer[VERTEX_BUFFER::MAGE]);
			if (FAILED(hr))
			{
				MessageBox(nullptr, L"mage v buffer creation failed", L"Error", MB_OK);
				exit(0);
			}

			// BattleMage index buffer
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(uint32_t) * skinnedMesh.index_count;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;

			ZeroMemory(&sdd, sizeof(sdd));
			sdd.pSysMem = skinnedMesh.indices;

			hr = device->CreateBuffer(&bd, &sdd, &index_buffer[INDEX_BUFFER::MAGE]);
			if (hr != S_OK)
			{
				MessageBox(nullptr, L"Battlemage Index buffer failed", L"Error", MB_OK);
				exit(0);
			}

			XMMATRIX animMat;
			for (size_t i = 0; i < qSize; i++)
			{
				animMat = XMMatrixRotationQuaternion(XMLoadFloat4(&animation.frames[0].joints[i].rotation)) * XMMatrixTranslationFromVector(XMLoadFloat4(&animation.frames[0].joints[i].pos));
				matrices[i] =  qs[i] * animMat;
			}

#pragma endregion

#pragma region TextureLoading

			if (diffusePath)
			{
				hr = DirectX::CreateWICTextureFromFile(device, diffusePath, nullptr, &resourceView[SHADER_RESOURCE_VIEW::DIFFUSE]);
				if (hr != S_OK)
				{
					MessageBox(nullptr, L"Battlemage diffuse failed", L"Error", MB_OK);
					exit(0);
				}
			}

			if (specularPath)
			{
				hr = DirectX::CreateWICTextureFromFile(device, specularPath, nullptr, &resourceView[SHADER_RESOURCE_VIEW::SPECULAR]);
				if (hr != S_OK)
				{
					MessageBox(nullptr, L"Battlemage specular failed", L"Error", MB_OK);
					exit(0);
				}
			}

			if (emissivePath)
			{
				hr = DirectX::CreateWICTextureFromFile(device, emissivePath, nullptr, &resourceView[SHADER_RESOURCE_VIEW::EMISSIVE]);
				if (hr != S_OK)
				{
					MessageBox(nullptr, L"Battlemage emissive failed", L"Error", MB_OK);
					exit(0);
				}
			}

			if (NormalPath)
			{
				hr = DirectX::CreateWICTextureFromFile(device, NormalPath, nullptr, &resourceView[SHADER_RESOURCE_VIEW::NORMAL]);
				if (hr != S_OK)
				{
					MessageBox(nullptr, L"Normal failed", L"Error", MB_OK);
					exit(0);
				}
			}


#pragma endregion

#pragma region SamplerState
			D3D11_SAMPLER_DESC sampDesc = {};
			sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
			sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
			sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
			sampDesc.MinLOD = 0;
			sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

			hr = device->CreateSamplerState(&sampDesc, &samplerState[SAMPLER_STATE::MAGE]);
			if (hr != S_OK)
			{
				MessageBox(nullptr, L"Sampler state creation failed", L"Error", MB_OK);
				exit(0);
			}
#pragma endregion
		}

		void draw_view(view_t& v)
		{
			timer.Signal();

			if (GetAsyncKeyState('C') & 0x1)
			{
				rasterStatenum++;
				if (rasterStatenum >= STATE_RASTERIZER::COUNT)
				{
					rasterStatenum = 0;
				}
				switch (rasterStatenum)
				{
				case STATE_RASTERIZER::DEFAULT:
				{
					context->RSSetState(rasterState[STATE_RASTERIZER::DEFAULT]);
					break;
				}

				case STATE_RASTERIZER::CULLFRONT:
				{
					context->RSSetState(rasterState[STATE_RASTERIZER::CULLFRONT]);
					break;
				}

				//case STATE_RASTERIZER::NONE:
				{
					context->RSSetState(rasterState[STATE_RASTERIZER::NONE]);
					break;
				}

				case STATE_RASTERIZER::WIREFRAME:
				{
					context->RSSetState(rasterState[STATE_RASTERIZER::WIREFRAME]);
					break;
				}

				}

			}

#pragma region ContextSetting

			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->IASetInputLayout(input_layout[INPUT_LAYOUT::ENDRENDERER]);

			context->OMSetRenderTargets(1, &render_target[VIEW_RENDER_TARGET::DEFAULT], depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT]);
			context->OMSetDepthStencilState(depthStencilState[STATE_DEPTH_STENCIL::DEFAULT], 0);

			context->RSSetViewports(1, &view_port[VIEWPORT::DEFAULT]);
			const float color[] = { 0.0f, 0.0f, 0.2f, 1 };
			context->ClearRenderTargetView(render_target[0], color);
			context->ClearDepthStencilView(depthStencilView[VIEW_DEPTH_STENCIL::DEFAULT], D3D11_CLEAR_DEPTH, 1, 0);
#pragma endregion

#pragma region RotatingLight

			XMFLOAT4 lightDir = { 10, 10, 10, 1 };
			XMMATRIX rotate = XMMatrixRotationY((float)timer.TotalTime() * 1.5f);
			XMVECTOR vLightDir = XMLoadFloat4(&lightDir);
			vLightDir = XMVector4Transform(vLightDir, rotate);
			XMStoreFloat4(&lightDir, vLightDir);
#pragma endregion

			ZeroMemory(&rs, sizeof(rs));
			cb.world = XMMatrixTranslationFromVector(vLightDir);
			cb.view = XMMatrixInverse(nullptr, v.view);
			cb.proj = v.proj;
			memcpy(&cb.matrices, matrices, sizeof(cb.matrices));

#pragma region Cube
			context->Map(constant_buffer[CONSTANT_BUFFER::MVP], 0, D3D11_MAP_WRITE_DISCARD, 0, &rs);
			memcpy(rs.pData, &cb, sizeof(cb));
			context->Unmap(constant_buffer[CONSTANT_BUFFER::MVP], 0);

			context->VSSetShader(vertex_shader[VERTEX_SHADER::ENDRENDERER], nullptr, 0);
			context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::MVP]);
			context->PSSetShader(pixel_shader[PIXEL_SHADER::ENDRENDERER], nullptr, 0);

			context->Draw(36, 0);
#pragma endregion

#pragma region DebugLines

			UINT stride = sizeof(colored_vertex);
			UINT offset = 0;

			cb.world = GridWM;
			context->Map(constant_buffer[CONSTANT_BUFFER::MVP], 0, D3D11_MAP_WRITE_DISCARD, 0, &rs);
			memcpy(rs.pData, &cb, sizeof(cb));
			context->Unmap(constant_buffer[CONSTANT_BUFFER::MVP], 0);

			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			context->IASetVertexBuffers(0, 1, &vertex_buffer[VERTEX_BUFFER::COLORED_VERTEX], &stride, &offset);
			context->IASetInputLayout(input_layout[INPUT_LAYOUT::COLORED_VERTEX]);
			context->VSSetShader(vertex_shader[VERTEX_SHADER::COLORED_VERTEX], nullptr, 0);
			context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::MVP]);
			context->PSSetShader(pixel_shader[PIXEL_SHADER::COLORED_VERTEX], nullptr, 0);

			context->Map(vertex_buffer[VERTEX_BUFFER::COLORED_VERTEX], 0, D3D11_MAP_WRITE_DISCARD, 0, &rs);
			memcpy(rs.pData, debug_renderer::get_line_verts(), sizeof(colored_vertex) * debug_renderer::get_line_vert_count());
			context->Unmap(vertex_buffer[VERTEX_BUFFER::COLORED_VERTEX], 0);
			// draws debug lines
			context->Draw((UINT)debug_renderer::get_line_vert_count(), 0);

#pragma endregion

#pragma region Model
			// Draw Model
			//cb.world = XMMatrixScaling(3.25f, 3.25f, 3.25f);

			context->Map(constant_buffer[CONSTANT_BUFFER::MVP], 0, D3D11_MAP_WRITE_DISCARD, 0, &rs);
			memcpy(rs.pData, &cb, sizeof(cb));
			context->Unmap(constant_buffer[CONSTANT_BUFFER::MVP], 0);

			XMFLOAT4 campos;
			XMStoreFloat4(&campos, v.view.r[3]);
			psCB pscb = { lightDir , campos };

			context->Map(constant_buffer[CONSTANT_BUFFER::PIXEL], 0, D3D11_MAP_WRITE_DISCARD, 0, &rs);
			memcpy(rs.pData, &pscb, sizeof(pscb));
			context->Unmap(constant_buffer[CONSTANT_BUFFER::PIXEL], 0);

			stride = sizeof(Skinned_vert);
			
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->IASetVertexBuffers(0, 1, &vertex_buffer[VERTEX_BUFFER::MAGE], &stride, &offset);
			context->IASetInputLayout(input_layout[INPUT_LAYOUT::MAGE]);
			context->IASetIndexBuffer(index_buffer[INDEX_BUFFER::MAGE], DXGI_FORMAT_R32_UINT, 0);
			ID3D11ShaderResourceView* texs[] = {
				resourceView[SHADER_RESOURCE_VIEW::DIFFUSE],
				resourceView[SHADER_RESOURCE_VIEW::SPECULAR],
				resourceView[SHADER_RESOURCE_VIEW::EMISSIVE],
				resourceView[SHADER_RESOURCE_VIEW::NORMAL] };
			context->PSSetShaderResources(0, _ARRAYSIZE(texs), texs);

			context->VSSetShader(vertex_shader[VERTEX_SHADER::MAGE], nullptr, 0);
			context->VSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::MVP]);

			context->PSSetShader(pixel_shader[PIXEL_SHADER::MAGE], nullptr, 0);
			context->PSSetConstantBuffers(0, 1, &constant_buffer[CONSTANT_BUFFER::PIXEL]);
			context->PSSetSamplers(0, 1, &samplerState[SAMPLER_STATE::MAGE]);

			context->DrawIndexed(skinnedMesh.index_count, 0, 0);

			context->PSSetShaderResources(0, 0, nullptr);
			context->PSSetSamplers(0, 0, nullptr);
#pragma endregion

			swapchain->Present(0, 0);
		}

		void AnimationStep(int dir)
		{
			animStep += dir;
			if (animStep < 0)
			{
				animStep = (int)maxFrames - 1;
			}
			else if (animStep >= maxFrames)
			{
				animStep = 0;
			}
		}

		void matricesAxis(XMMATRIX mats[3])
		{
			XMFLOAT3 pos, axi;
			for (int i = 0; i < 3; i++)
			{
				// x axis
				XMStoreFloat3(&pos, mats[i].r[3]);
				XMStoreFloat3(&axi, mats[i].r[0]);
				XMStoreFloat3(&axi, XMLoadFloat3(&axi) + XMLoadFloat3(&pos));
				debug_renderer::add_line(pos, axi, XMFLOAT4(1, 0, 0, 1));

				// y axis
				XMStoreFloat3(&pos, mats[i].r[3]);
				XMStoreFloat3(&axi, mats[i].r[1]);
				XMStoreFloat3(&axi, XMLoadFloat3(&axi) + XMLoadFloat3(&pos));
				debug_renderer::add_line(pos, axi, XMFLOAT4(0, 1, 0, 1));

				// z axis
				XMStoreFloat3(&pos, mats[i].r[3]);
				XMStoreFloat3(&axi, mats[i].r[2]);
				XMStoreFloat3(&axi, XMLoadFloat3(&axi) + XMLoadFloat3(&pos));
				debug_renderer::add_line(pos, axi, XMFLOAT4(0, 0, 1, 1));
			}
		}

		XMMATRIX LookAt(XMVECTOR viewer, XMVECTOR target, XMVECTOR up)
		{
			XMVECTOR z = XMVector3Normalize(target - viewer);
			XMVECTOR x = XMVector3Normalize(XMVector3Cross(z,up));
			XMVECTOR y = XMVector3Normalize(XMVector3Cross(x, z));

			XMMATRIX mat = { -x, y, z, viewer };

			return mat;
		}

		XMMATRIX TurnTo(XMMATRIX m, XMVECTOR target, float speed)
		{
			XMMATRIX returnMat = m;
			XMVECTOR wheretolook = target - m.r[3];
			wheretolook = XMVector3Normalize(wheretolook);
			float dotX = XMVector3Dot(wheretolook, m.r[0]).m128_f32[0];
			float dotY = XMVector3Dot(wheretolook, m.r[1]).m128_f32[0];
			float dotZ = XMVector3Dot(wheretolook, m.r[2]).m128_f32[0];
			returnMat = XMMatrixRotationX(XMConvertToRadians(-dotY * speed)) *  returnMat;

			XMVECTOR pos = returnMat.r[3];
			returnMat.r[3] = { 0, 0, 0 };
			returnMat *= XMMatrixRotationY(XMConvertToRadians(dotX* speed));
			returnMat.r[3] = pos;


			return returnMat;
		}

		void MouseRotation(view_t &v, int x, int y, float delta)
		{
			float rotationSpeedY = 27.0f;
			float rotationSpeedX = 26.0f;
			XMVECTOR scale;
			XMVECTOR pos;
			XMVECTOR rot;
			XMVECTOR reset = { 0,0,0 };
			XMMatrixDecompose(&scale, &rot, &pos, v.view);
			v.view.r[3] = reset;
			v.view *= XMMatrixRotationY(XMConvertToRadians(x * delta * rotationSpeedY));
			v.view.r[3] = pos;


			v.view = XMMatrixRotationX(XMConvertToRadians(y * delta * rotationSpeedX)) * v.view;
		}

		void ClearDebugLines()
		{
			debug_renderer::clear_lines();
	//		Skeletons();
		}

		void PlayAnimation(bool play)
		{
			if (play)
				animTimer.Signal();
		}

		void DebugGrid(float time)
		{
			float timex = fabsf(cosf(time));
			float timey = fabsf(sinf(time));
			float timez = timey + timex;

			XMFLOAT4 colorLerping = { timex, timey, timez, 1 }, grey = { 0.25f, 0.25f, 0.25f, 1 }, white = { 1,1,1,1 };
			XMFLOAT4 colorToDraw = grey;


			float length = 10;
			float offset = length * 0.5f;
			for (float i = -offset; i <= offset; i++)
			{
				if (fmodf(i,10) == 0)
				{
					colorToDraw = white;
				}
				else
				{
					colorToDraw = grey;
				}
				debug_renderer::add_line(XMFLOAT3(-offset, 0, i), XMFLOAT3(offset, 0, i), colorToDraw);
				debug_renderer::add_line(XMFLOAT3(i, 0,-offset), XMFLOAT3(i, 0,offset), colorToDraw);
			}
		}

		void Skeletons()
		{
			XMFLOAT3 one, two;
			XMVECTOR parentLerp, posLerp, rotLerp;

			for (size_t i = 0; i < qSize; i++)
			{
				int parent = animation.frames[animStep].joints[i].parent_index;
				rotLerp = XMVectorLerp(XMLoadFloat4(&animation.frames[animStep].joints[i].rotation), XMLoadFloat4(&animation.frames[(animStep + 1) % maxFrames].joints[i].rotation), 0);
				posLerp = XMVectorLerp(XMLoadFloat4(&animation.frames[animStep].joints[i].pos), XMLoadFloat4(&animation.frames[(animStep + 1) % maxFrames].joints[i].pos), 0);

				if (parent != -1)
				{
					parentLerp = XMVectorLerp(XMLoadFloat4(&animation.frames[animStep].joints[parent].pos), XMLoadFloat4(&animation.frames[(animStep + 1) % maxFrames].joints[parent].pos), 0);
					// animated in model drawn white
					XMMATRIX matrix = XMMatrixScaling(1, 1, 1);
					XMStoreFloat3(&one, XMVector3Transform(posLerp, matrix));
					XMStoreFloat3(&two, XMVector3Transform(parentLerp, matrix));
					debug_renderer::add_line(one, two, XMFLOAT4(1, 1, 1, 1));

					// animated moved to the side and drawn red
					matrix =  XMMatrixTranslation(15, 0, 0);
					posLerp = XMVector3Transform(posLerp, matrix);
					parentLerp = XMVector3Transform(parentLerp, matrix);
					XMStoreFloat3(&one, posLerp);
					XMStoreFloat3(&two, parentLerp);
					debug_renderer::add_line(one, two, XMFLOAT4(1, 0, 0, 1));

					// Bind pose drawn in green
					XMVECTOR vPos, vPos2, vRot, vScale;
					XMMATRIX mFirst = XMMatrixInverse(nullptr, qs[i]);
					XMMATRIX mParent = XMMatrixInverse(nullptr, qs[parent]);
					XMMatrixDecompose(&vScale, &vRot, &vPos, mFirst);
					XMMatrixDecompose(&vScale, &vRot, &vPos2, mParent);

					matrix = XMMatrixTranslation(-15, 0, 0);
					vPos = XMVector3Transform(vPos,mFirst * matrix);
					vPos2 = XMVector3Transform(vPos2, mParent * matrix);
					XMStoreFloat3(&one, vPos);
					XMStoreFloat3(&two, vPos2);
					debug_renderer::add_line(one, two, XMFLOAT4(0, 1, 0, 1));
				}
			}
		}

		void debug_lines(float time)
		{
			animTimer.Signal();
			float currentTime = (float)animTimer.TotalTime();
			if (currentTime > animation.duration)
			{
				animTimer.Restart();
				animStep = 0;
				currentTime = 0;
			}
			float axiScale = 0.15f;
			float Ratio = 0;

			if (animStep != maxFrames - 1)
			{
				Ratio = (float)(currentTime - animation.frames[animStep - 1].time) / float(animation.frames[(animStep + 1) % maxFrames].time - animation.frames[animStep].time);
			}
			else
			{
				Ratio = (currentTime) / float(animation.duration - animation.frames[animStep - 1].time) - animStep;
			}


			XMFLOAT3 pos, axi;
			XMVECTOR parentLerp, posLerp, rotLerp;
			XMMATRIX animMat;
			for (size_t i = 0; i < qSize; i++)
			{
				int parent = animation.frames[animStep].joints[i].parent_index;
				Ratio = 0;
				rotLerp = XMVectorLerp(XMLoadFloat4(&animation.frames[animStep].joints[i].rotation), XMLoadFloat4(&animation.frames[(animStep + 1) % maxFrames].joints[i].rotation), Ratio);
				posLerp = XMVectorLerp(XMLoadFloat4(&animation.frames[animStep].joints[i].pos), XMLoadFloat4(&animation.frames[(animStep + 1) % maxFrames].joints[i].pos), Ratio);

				parentLerp = XMVectorLerp(XMLoadFloat4(&animation.frames[animStep].joints[parent].pos), XMLoadFloat4(&animation.frames[(animStep + 1) % maxFrames].joints[parent].pos), Ratio);


				animMat = XMMatrixRotationQuaternion(rotLerp) * XMMatrixTranslationFromVector(posLerp);
				matrices[i] = qs[i] * animMat;

				{
					// x axis
					XMStoreFloat3(&pos, animMat.r[3]);
					XMStoreFloat3(&axi, animMat.r[0] * axiScale);
					XMStoreFloat3(&axi, XMLoadFloat3(&axi) + XMLoadFloat3(&pos));
					debug_renderer::add_line(pos, axi, XMFLOAT4(1, 0, 0, 1));

					// y axis
					XMStoreFloat3(&pos, animMat.r[3]);
					XMStoreFloat3(&axi, animMat.r[1] * axiScale);
					XMStoreFloat3(&axi, XMLoadFloat3(&axi) + XMLoadFloat3(&pos));
					debug_renderer::add_line(pos, axi, XMFLOAT4(0, 1, 0, 1));

					// z axis
					XMStoreFloat3(&pos, animMat.r[3]);
					XMStoreFloat3(&axi, animMat.r[2] * axiScale);
					XMStoreFloat3(&axi, XMLoadFloat3(&axi) + XMLoadFloat3(&pos));
					debug_renderer::add_line(pos, axi, XMFLOAT4(0, 0, 1, 1));
				}
			}

			if (animation.frames[animStep].time < currentTime)// && animation.frames[(animStep + 1) % maxFrames].time < totalTime) && animStep != maxFrames - 1)
			{
				if (animStep < maxFrames - 1)
					animStep++;
			}
		}

		void TerrainTriangles(std::vector<XMFLOAT3> tris)
		{
			for (size_t i = 0; i < tris.size() - 3; i += 3)
			{
				debug_renderer::add_line(tris[i], tris[i + 1], { 1,1,1,1 });
				debug_renderer::add_line(tris[i + 1], tris[i + 2], { 1,1,1,1 });
				debug_renderer::add_line(tris[i], tris[i + 2], { 1,1,1,1 });
			}
		}

		void particles(XMFLOAT3 &pos, XMFLOAT3& vel, XMFLOAT4 col, float delta)
		{
			XMFLOAT3 prev = pos;
			XMStoreFloat3(&pos, XMVectorAdd(XMLoadFloat3(&pos), XMLoadFloat3(&vel) * delta));

			//	XMStoreFloat3(&pos, XMVectorScale(XMLoadFloat3(&pos), 1.0001));

			debug_renderer::add_line(pos, prev, col);
		}

		void CameraMovement(view_t& v, float delta)
		{
			int cameraSpeed = 14;
			float rotationSpeed = 1.25f;

			if (GetAsyncKeyState('W'))
				v.view = XMMatrixTranslation(0, 0, delta * cameraSpeed) * v.view;

			if (GetAsyncKeyState('S'))
				v.view = XMMatrixTranslation(0, 0, -delta * cameraSpeed)* v.view;

			if (GetAsyncKeyState('A'))
				v.view = XMMatrixTranslation(-delta * cameraSpeed, 0, 0)* v.view;

			if (GetAsyncKeyState('D'))
				v.view = XMMatrixTranslation(delta * cameraSpeed, 0, 0)* v.view;


			if (GetAsyncKeyState(VK_SPACE))
				v.view *= XMMatrixTranslation(0, delta * cameraSpeed, 0);

			if (GetAsyncKeyState(VK_CONTROL))
				v.view *= XMMatrixTranslation(0, -delta * cameraSpeed, 0);

		/*	if (GetAsyncKeyState(VK_LEFT))
			{
				XMVECTOR scale;
				XMVECTOR pos;
				XMVECTOR rot;
				XMVECTOR reset = { 0,0,0 };
				XMMatrixDecompose(&scale, &rot, &pos, v.view);
				v.view.r[3] = reset;
				v.view *= XMMatrixRotationY(-delta * rotationSpeed);
				v.view.r[3] = pos;
			}

			if (GetAsyncKeyState(VK_RIGHT))
			{
				XMVECTOR scale;
				XMVECTOR pos;
				XMVECTOR rot;
				XMVECTOR reset = { 0,0,0 };
				XMMatrixDecompose(&scale, &rot, &pos, v.view);
				v.view.r[3] = reset;
				v.view *= XMMatrixRotationY(delta * rotationSpeed);
				v.view.r[3] = pos;
			}

			if (GetAsyncKeyState(VK_UP))
				v.view = XMMatrixRotationX(-delta * rotationSpeed) * v.view;

			if (GetAsyncKeyState(VK_DOWN))
				v.view = XMMatrixRotationX(delta * rotationSpeed) * v.view;
				*/
		}

		~impl()
		{
			// TODO:
			//Clean-up
			//
			// In general, release objects in reverse order of creation

			int i;
			for (i = 0; i < STATE_RASTERIZER::COUNT; i++)		if (rasterState[i]) rasterState[i]->Release();
			for (i = 0; i < PIXEL_SHADER::COUNT; i++)			if (pixel_shader[i]) pixel_shader[i]->Release();
			for (i = 0; i < VERTEX_SHADER::COUNT; i++)			if (vertex_shader[i]) vertex_shader[i]->Release();
			for (i = 0; i < CONSTANT_BUFFER::COUNT; i++)		if (constant_buffer[i]) constant_buffer[i]->Release();
			for (i = 0; i < INPUT_LAYOUT::COUNT; i++)			if (input_layout[i]) input_layout[i]->Release();
			for (i = 0; i < INDEX_BUFFER::COUNT; i++)			if (index_buffer[i]) index_buffer[i]->Release();
			for (i = 0; i < VERTEX_BUFFER::COUNT; i++)			if (vertex_buffer[i]) vertex_buffer[i]->Release();
			for (i = 0; i < STATE_DEPTH_STENCIL::COUNT; i++)	if (depthStencilState[i]) depthStencilState[i]->Release();
			for (i = 0; i < VIEW_DEPTH_STENCIL::COUNT; i++)		if (depthStencilView[i]) depthStencilView[i]->Release();
			for (i = 0; i < VIEW_RENDER_TARGET::COUNT; i++)		if (render_target[i]) render_target[i]->Release();
			for (i = 0; i < SHADER_RESOURCE_VIEW::COUNT; i++)	if (resourceView[i]) resourceView[i]->Release();
			for (i = 0; i < SAMPLER_STATE::COUNT; i++)			if (samplerState[i]) samplerState[i]->Release();
			for (i = 0; i < TEXTURE::COUNT; i++)				if (texture[i]) texture[i]->Release();

			delete[] skinnedMesh.verts;
			delete[] skinnedMesh.indices;
			for (size_t i = 0; i < maxFrames; i++)
			{
				delete animation.frames[i].joints;
			}
			delete animation.frames;
			delete qs;


			if (swapchain)swapchain->Release();
			if (context)context->Release();
			if (device)device->Release();
		}
	};
}