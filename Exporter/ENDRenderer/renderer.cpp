#include "renderer.h"

#ifdef FSGD_END_USE_D3D
#include "d3d11_renderer_impl.h"
#endif

namespace end
{
	renderer::renderer(native_handle_type window_handle)
	{
		p_impl = new impl(window_handle, default_view);//create and initialize the implementation
	}

	renderer::~renderer()
	{
		// Clean up implementation
		delete p_impl;
	}

	void renderer::draw()
	{
		// draw views...
		p_impl->draw_view(default_view);
		// draw views
		// draw views...
	}

	void renderer::ClearDebugLines()
	{
		p_impl->ClearDebugLines();
	}

	void renderer::debug_lines(float time)
	{
		p_impl->debug_lines(time);
	}

	void renderer::DebugGrid(float time)
	{
		p_impl->DebugGrid(time);
	}

	void renderer::CameraMovement(float time)
	{
		p_impl->CameraMovement(default_view, time);
	}

	void renderer::particles(XMFLOAT3 &pos, XMFLOAT3 &vel, XMFLOAT4 col, float delta)
	{
		p_impl->particles(pos, vel, col, delta);
	}

	void renderer::matricesAxis(XMMATRIX mats[3])
	{
		p_impl->matricesAxis(mats);
	}

	XMMATRIX renderer::LookAt(XMVECTOR viewer, XMVECTOR target, XMVECTOR up)
	{
		return p_impl->LookAt(viewer, target, up);
	}

	XMMATRIX renderer::TurnTo(XMMATRIX m, XMVECTOR target, float speed)
	{
		return p_impl->TurnTo(m, target, speed);
	}

	void renderer::MouseRotation(int x, int y, float delta)
	{
		p_impl->MouseRotation(default_view, x, y , delta);
	}

	void renderer::TerrainTriangles(std::vector<XMFLOAT3> tris)
	{
		p_impl->TerrainTriangles(tris);
	}

	void renderer::AnimationStep(int x)
	{
		p_impl->AnimationStep(x);
	}

}