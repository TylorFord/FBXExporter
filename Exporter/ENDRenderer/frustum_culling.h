#pragma once


#include <array>

// Note: You are free to make adjustments/additions to the declarations provided here.
#include "debug_renderer.h"
#include "view.h"
#include <DirectXMath.h>
using namespace DirectX;
namespace end
{
	struct sphere_t { XMVECTOR center; float radius; }; //Alterative: using sphere_t = float4;

	struct aabb_t
	{
		XMVECTOR min; XMVECTOR max; XMFLOAT4 color;
	}; //Alternative: aabb_t { XMFLOAT3 min; XMFLOAT3 max; };

	struct plane_t { XMVECTOR normal; float offset; };  //Alterative: using plane_t = float4;

	using frustum_t = std::array<plane_t, 6>;

	// Calculates the plane of a triangle from three points.
	plane_t calculate_plane(XMVECTOR a, XMVECTOR b, XMVECTOR c, XMVECTOR d);

	// Calculates a frustum (6 planes) from the input view parameter.
	//
	// Calculate the eight corner points of the frustum. 
	// Use your debug renderer to draw the edges.
	// 
	// Calculate the frustum planes.
	// Use your debug renderer to draw the plane normals as line segments.
	void calculate_frustum(frustum_t& frustum, const XMMATRIX& view);

	// Calculates which side of a plane the sphere is on.
	//
	// Returns -1 if the sphere is completely behind the plane.
	// Returns 1 if the sphere is completely in front of the plane.
	// Otherwise returns 0 (Sphere overlaps the plane)
	int classify_sphere_to_plane(const sphere_t& sphere, const plane_t& plane);

	// Calculates which side of a plane the aabb is on.
	//
	// Returns -1 if the aabb is completely behind the plane.
	// Returns 1 if the aabb is completely in front of the plane.
	// Otherwise returns 0 (aabb overlaps the plane)
	// MUST BE IMPLEMENTED UsING THE PROJECTED RADIUS TEST
	int classify_aabb_to_plane(const aabb_t& aabb, const plane_t& plane);

	// Determines if the aabb is inside the frustum.
	//
	// Returns false if the aabb is completely behind any plane.
	// Otherwise returns true.
	bool aabb_to_frustum(const aabb_t& aabb, const frustum_t& frustum);

	void drawAABB(aabb_t);
}