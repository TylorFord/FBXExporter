#pragma once
#include <memory>
#include "view.h"
#include <vector>

#define FSGD_END_USE_D3D

namespace end
{
	// APIs/Platforms have their own types for representing a handle to a 'window'
	// They will generally all fit in the 8-bytes of a void*.
	// HWND is actually just a typedef/alias for a void*.
	using native_handle_type = void*;

	// Interface to the renderer
	class renderer
	{
	public:

		renderer(native_handle_type window_handle);

		~renderer();

		void draw();

		void ClearDebugLines();

		void debug_lines(float time);

		void DebugGrid(float time);

		void CameraMovement(float time);

		void particles(XMFLOAT3& pos, XMFLOAT3 &vel, XMFLOAT4 col, float delta);

		void matricesAxis(XMMATRIX mats[3]);

		XMMATRIX LookAt(XMVECTOR viewer, XMVECTOR target, XMVECTOR up);

		XMMATRIX TurnTo(XMMATRIX m, XMVECTOR target, float speed);

		void MouseRotation(int x, int y, float);

		void TerrainTriangles(std::vector<XMFLOAT3> tris);

		void AnimationStep(int);

		view_t default_view;



	private:

		// PImpl idiom ("Pointer to implementation")

		// 'impl' will define the implementation for the renderer elsewhere
		struct impl;

		// Pointer to the implementation
		impl* p_impl;
	};

	// The following types just defines scopes for enum values.
	// The enum values can be used as indices in arrays.
	// These enum values can be added to as needed.

	struct VIEWPORT
	{
		enum
		{
			DEFAULT = 0, COUNT
		};
	};

	struct CONSTANT_BUFFER
	{
		enum
		{
			MVP = 0, PIXEL, COUNT
		};
	};

	struct VERTEX_SHADER
	{
		enum
		{
			COLORED_VERTEX = 0, ENDRENDERER, MAGE, SIMPLEMESH, COUNT
		};
	};

	struct PIXEL_SHADER
	{
		enum
		{
			COLORED_VERTEX = 0, ENDRENDERER, MAGE, SIMPLEMESH, COUNT
		};
	};

	struct VIEW_RENDER_TARGET
	{
		enum
		{
			DEFAULT = 0, COUNT
		};
	};

	struct INDEX_BUFFER
	{
		enum
		{
			DEFAULT = 0, MAGE, COUNT
		};
	};

	struct INPUT_LAYOUT
	{
		enum
		{
			COLORED_VERTEX = 0, ENDRENDERER, MAGE, SIMPLEMESH,COUNT
		};
	};

	struct STATE_RASTERIZER
	{
		enum
		{
			DEFAULT = 0, CULLFRONT, NONE, WIREFRAME, COUNT
		};
	};

	struct VIEW_DEPTH_STENCIL
	{
		enum
		{
			DEFAULT = 0, COUNT
		};
	};

	struct STATE_DEPTH_STENCIL
	{
		enum
		{
			DEFAULT = 0, COUNT
		};
	};

	struct VERTEX_BUFFER
	{
		enum
		{
			COLORED_VERTEX = 0, TERRAIN, MAGE, SIMPLEMESH,COUNT
		};
	};

	struct SHADER_RESOURCE_VIEW
	{
		enum
		{
			DIFFUSE = 0, SPECULAR, EMISSIVE, NORMAL, COUNT
		};
	};

	struct SAMPLER_STATE
	{
		enum
		{
			MAGE = 0, COUNT
		};
	};

	struct TEXTURE
	{
		enum
		{
			MAGE = 0, COUNT
		};
	};



	/* Add more as needed...
	enum STATE_SAMPLER{ DEFAULT = 0, COUNT };

	enum STATE_BLEND{ DEFAULT = 0, COUNT };
	*/
}