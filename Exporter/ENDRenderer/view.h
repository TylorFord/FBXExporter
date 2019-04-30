#pragma once

#include "math_types.h"
#define JOINTCOUNT 57

namespace end
{
	class view_t
	{
		// stores properties of a view
		//
		//	view and projection matrices	(REQUIRED)
		//	type information 				(optional) (Orthographic/Perspective/Cubemap/Shadowmap/etc)
		//	render target id(s)				(optional)
		//	viewport id						(optional)
		//		IMPORTANT: 
		//			Do not store an inverted view matrix.
		//			It will be much easier on you, me, and the CPU this way.
		//			When updating your constant buffers, send an inverted copy of the stored view matrix.
	public:

		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX proj;
		XMMATRIX matrices[JOINTCOUNT];

		// maintains a visible-set of renderable objects in view (implemented in a future assignment)
			   
		view_t(){}

		view_t(XMMATRIX _world, XMMATRIX _view, XMMATRIX _proj)
		{
			world = _world;
			view = _view;
			proj = _proj;
		}
	};
}