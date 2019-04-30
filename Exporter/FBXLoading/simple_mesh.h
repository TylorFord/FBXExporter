#pragma once
#include <fbxsdk.h>
#include <DirectXMath.h>
#include <vector>
#include <array>
#define EPSILON 0.00001f
namespace end
{
	struct simple_vert
	{
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT3 norm;
		DirectX::XMFLOAT2 tex;

		DirectX::XMFLOAT4 tangents;

		bool operator==(const simple_vert &n) const
		{
			if ((pos.x != n.pos.x) ||
				(pos.y != n.pos.y) ||
				(pos.z != n.pos.z) ||
				(tex.x != n.tex.x) ||
				(tex.y != n.tex.y) ||
				(norm.x != n.norm.x) ||
				(norm.y != n.norm.y) ||
				(norm.z != n.norm.z))
			{
				return false;
			}
			else
				return true;
		}
	};

	struct skinned_vert
	{
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT3 norm;
		DirectX::XMFLOAT2 tex;
		DirectX::XMFLOAT4 indices;
		DirectX::XMFLOAT4 weights;

		DirectX::XMFLOAT4 tangents;
		//DirectX::XMFLOAT4 binormals;

		bool operator==(const skinned_vert &n) const
		{
			if ((pos.x != n.pos.x) ||
				(pos.y != n.pos.y) ||
				(pos.z != n.pos.z) ||
				(tex.x != n.tex.x) ||
				(tex.y != n.tex.y) ||
				(norm.x != n.norm.x) ||
				(norm.y != n.norm.y) ||
				(norm.z != n.norm.z) ||
				(indices.x != n.indices.x) ||
				(indices.y != n.indices.y) ||
				(indices.z != n.indices.z) ||
				(indices.w != n.indices.w) ||
				(weights.x != n.weights.x) ||
				(weights.y != n.weights.y) ||
				(weights.z != n.weights.z) ||
				(weights.w != n.weights.w))
			{
				return false;
			}
			else
				return true;
		}
	};

	struct Hash
	{
		size_t operator()(const skinned_vert v) const
		{
			size_t pos = std::hash<float>()(v.pos.x) ^ std::hash<float>()(v.pos.y) ^ std::hash<float>()(v.pos.z) ^ std::hash<float>()(v.pos.w);
			size_t norm = std::hash<float>()(v.norm.x) ^ std::hash<float>()(v.norm.y) ^ std::hash<float>()(v.norm.z);
			size_t tex = std::hash<float>()(v.tex.x) ^ std::hash<float>()(v.tex.y);
			return (pos ^ norm ^ tex);
		}

		size_t operator()(const simple_vert v) const
		{
			size_t pos = std::hash<float>()(v.pos.x) ^ std::hash<float>()(v.pos.y) ^ std::hash<float>()(v.pos.z) ^ std::hash<float>()(v.pos.w);
			size_t norm = std::hash<float>()(v.norm.x) ^ std::hash<float>()(v.norm.y) ^ std::hash<float>()(v.norm.z);
			size_t tex = std::hash<float>()(v.tex.x) ^ std::hash<float>()(v.tex.y);
			return (pos ^ norm ^ tex);
		}
	};

	struct simple_mesh
	{
		uint32_t vert_count = 0;
		uint32_t index_count = 0;
		simple_vert* verts = nullptr;
		uint32_t* indices = nullptr;
	};

	struct skinned_mesh
	{
		uint32_t vert_count = 0;
		uint32_t index_count = 0;
		skinned_vert* verts = nullptr;
		uint32_t* indices = nullptr;

	};

	struct fbx_Joint
	{
		FbxNode *node = nullptr;
		int parent_index = 0;
	};

	struct Joint
	{
		DirectX::XMFLOAT4 pos;
		DirectX::XMFLOAT4 form;
		int parent_index;
	};

	struct KeyFrame
	{
		double time;
		std::vector<Joint> joints;
	};



	struct AnimClip
	{
		double duration;
		std::vector<KeyFrame> frames;
	};

	struct TJoint
	{
		DirectX::XMFLOAT4 m_vPos;
		DirectX::XMFLOAT4 m_vRotation;
		int m_nParentIndex;
	};

	struct TKeyframe
	{
		double m_dTime;
		TJoint* m_ptJoint;
	};

	struct TAnimationClip
	{
		double m_dDuration;
		int m_nKeyframeCount;
		TKeyframe* m_ptKeyframes;
		int GetCurrentFrame(double dCurrentTime);
		int GetNextKeyframe(int nCurrentFrame, double dCurrentTime);
	};

	struct influence
	{
		int joint;
		float weight;
	};
}
