#pragma once

#include "InterFace.h"
#include "simple_mesh.h"
#include "../DirectXTex-master/DirectXTex/DirectXTex.h"

#include <fbxsdk.h>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <string>

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new new( _CLIENT_BLOCK, __FILE__, __LINE__)
#endif

using namespace DirectX;
FbxManager * create_and_import(const char * fbx_file_path, FbxScene *& lScene)
{
	// Initialize the SDK manager. This object handles all our memory management.
	FbxManager* lSdkManager = FbxManager::Create();
	// Create the IO settings object.
	FbxIOSettings *ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);
	// Create an importer using the SDK manager.
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
	// Use the first argument as the filename for the importer.
	if (!lImporter->Initialize(fbx_file_path, -1, lSdkManager->GetIOSettings()))
	{
		printf("Call to FbxImporter::Initialize() failed.\n");
		printf(std::string(std::string(fbx_file_path) + std::string("\n")).c_str());
		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
		return nullptr;
	}
	// Create a new scene so that it can be populated by the imported file.
	lScene = FbxScene::Create(lSdkManager, "imported_scene");
	// Import the contents of the file into the scene.
	lImporter->Import(lScene);
	lImporter->Destroy();
	return lSdkManager;
}

FBXLOADING_API int get_scene_poly_count(const char* fbx_file_path)
{
	int result = -1;
	// Scene pointer, set by call to create_and_import
	FbxScene* scene = nullptr;
	// Create the FbxManager and import the scene from file
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);
	// Check if manager creation failed
	if (sdk_manager == nullptr)
		return result;
	//If the scene was imported...
	if (scene != nullptr)
	{
		//No errors to report, so start polygon count at 0
		result = 0;
		// Get the count of geometry objects in the scene
		int geo_count = scene->GetGeometryCount();
		for (int i = 0; i < geo_count; ++i)
		{
			//Get geometry number 'i'
			FbxGeometry* geo = scene->GetGeometry(i);
			// If it's not a mesh, skip it
			// Geometries might be some other type like nurbs
			if (geo->GetAttributeType() != FbxNodeAttribute::eMesh)
				continue;
			// Found a mesh, add its polygon count to the result
			FbxMesh* mesh = (FbxMesh*)geo;
			result += mesh->GetPolygonCount();
		}
	}
	//Destroy the manager
	sdk_manager->Destroy();
	//Return the polygon count for the scene
	return result;
}

FBXLOADING_API int export_simple_mesh(const char* fbx_file_path, const char* output_file_path, const char* mesh_name)
{
	int result = -1;
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);

	if (!scene)
		return result;
	FbxNode* childNode = nullptr;
	int childCount = scene->GetRootNode()->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		childNode = scene->GetRootNode()->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();
		if (mesh)
		{
			if (!mesh_name || mesh_name == mesh->GetName())
			{
				end::simple_mesh simpleMesh;

				int colorCount = mesh->GetElementVertexColorCount();
				// getting vertices and count from fbx
				simpleMesh.vert_count = mesh->GetControlPointsCount();
				simpleMesh.verts = new end::simple_vert[simpleMesh.vert_count];
				for (uint32_t j = 0; j < simpleMesh.vert_count; j++)
				{
					simpleMesh.verts[j].pos = DirectX::XMFLOAT4{ (float)mesh->GetControlPointAt(j).mData[0], (float)mesh->GetControlPointAt(j).mData[1], (float)mesh->GetControlPointAt(j).mData[2], (float)mesh->GetControlPointAt(j).mData[3] };
				}
				// getting indices array and count from fbx
				simpleMesh.index_count = mesh->GetPolygonVertexCount();
				simpleMesh.indices = (uint32_t*)mesh->GetPolygonVertices();

				// getting uv's from fbx
				DirectX::XMFLOAT2 * UV = new DirectX::XMFLOAT2[simpleMesh.index_count];
				for (int j = 0; j < mesh->GetPolygonCount(); j++)//polygon(=mostly rectangle) count
				{
					FbxLayerElementArrayTemplate<FbxVector2>* uvVertices = NULL;
					mesh->GetTextureUV(&uvVertices);
					for (int k = 0; k < mesh->GetPolygonSize(j); k++)//retrieves number of vertices in a polygon
					{
						FbxVector2 uv = uvVertices->GetAt(mesh->GetTextureUVIndex(j, k));
						UV[3 * j + k] = DirectX::XMFLOAT2((float)uv.mData[0], 1 - (float)uv.mData[1]);
					}
				}

				FbxLayerElementArrayTemplate<FbxVector4> *fbxTangents;
				mesh->GetTangents(&fbxTangents);


				// getting normals from fbx
				FbxArray<FbxVector4> normalsVec;
				mesh->GetPolygonVertexNormals(normalsVec);

				end::simple_vert* verts2 = new end::simple_vert[simpleMesh.index_count];
				simpleMesh.vert_count = simpleMesh.index_count;
				for (uint32_t j = 0; j < simpleMesh.index_count; j++)
				{
					verts2[j] = simpleMesh.verts[simpleMesh.indices[j]];
					verts2[j].norm = DirectX::XMFLOAT3{ (float)normalsVec[j].mData[0], (float)normalsVec[j].mData[1], (float)normalsVec[j].mData[2] };
					verts2[j].tex = UV[j];


					verts2[j].tangents = {
						(float)fbxTangents->GetAt(j).mData[0],
						(float)fbxTangents->GetAt(j).mData[1],
						(float)fbxTangents->GetAt(j).mData[2],
						(float)fbxTangents->GetAt(j).mData[3] };
				};

				delete[] simpleMesh.verts;
				simpleMesh.verts = verts2;

				unsigned int numIndices = 0;
				std::unordered_map<end::simple_vert, unsigned int, end::Hash> uniqueValues;
				std::vector<end::simple_vert> vertices;
				std::vector<unsigned int> indicesVector;

				for (uint32_t j = 0; j < simpleMesh.index_count; j++)
				{
					end::simple_vert v;
					v.norm = simpleMesh.verts[j].norm;
					v.pos = simpleMesh.verts[j].pos;
					v.tex = simpleMesh.verts[j].tex;

					if (uniqueValues.count(v) == 0)
					{
						uniqueValues.insert({ v, numIndices });
						vertices.push_back(v);
						indicesVector.push_back(numIndices++);
					}
					else
						indicesVector.push_back(uniqueValues[v]);
				}

				std::ofstream file(output_file_path, std::ios_base::binary | std::ios_base::trunc);
				if (file.is_open())
				{
					int size = (int)vertices.size();
					file.write((const char*)&size, sizeof(int));
					file.write((const char*)vertices.data(), sizeof(end::simple_vert) * size);
					size = (int)indicesVector.size();
					file.write((const char*)&size, sizeof(int));
					file.write((const char*)indicesVector.data(), sizeof(uint32_t) * size);
				}
				file.close();
				delete verts2;
				delete UV;
				return 0;
			}
		}
	}
	return result;
}

FBXLOADING_API int export_skinned_mesh(const char * fbx_file_path, const char * output_file_path, const char * mesh_name)
{
	int result = -1;
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);

	if (!scene)
		return result;
	FbxNode* childNode = nullptr;
	int childCount = scene->GetRootNode()->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		childNode = scene->GetRootNode()->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();
		if (mesh)
		{
			const char * name = mesh->GetName();
			if (!mesh_name || mesh_name == mesh->GetName())
			{
				
				const int MAX_INFLUENCES = 4;
				using influence_set = std::array<end::influence, MAX_INFLUENCES>;
				std::vector<influence_set> control_point_influences;
				control_point_influences.resize(mesh->GetControlPointsCount());
				std::vector<end::fbx_Joint> joints;

				int posecount = scene->GetPoseCount();
				FbxPose* pose = scene->GetPose(0);
				FbxMesh* poseMesh = pose->GetNode(0)->GetMesh();
				if (pose->IsBindPose())
				{
					posecount = pose->GetCount();
					for (int i = 0; i < posecount; i++)
					{
						FbxSkeleton* skele = pose->GetNode(i)->GetSkeleton();
						if (skele && skele->IsSkeletonRoot())
						{
							end::fbx_Joint joint;
							joint.node = pose->GetNode(i);
							joint.parent_index = -1;
							joints.push_back(joint);
							for (int j = 0; j < joints.size(); j++)
							{
								for (int k = 0; k < joints[j].node->GetChildCount(); k++)
								{
									skele = joints[j].node->GetChild(k)->GetSkeleton();
									if (skele)
									{
										joint.node = joints[j].node->GetChild(k);
										joint.parent_index = j;
										joints.push_back(joint);
									}
								}
							}
							break;
						}
					}

					int deformerCount = poseMesh->GetDeformerCount();
					for (int i = 0; i < deformerCount; i++)
					{
						FbxDeformer* skin = poseMesh->GetDeformer(i);
						if (skin->Is<FbxSkin>())
						{
							FbxSkin* newSkin = (FbxSkin*)skin;
							int clusterCount = newSkin->GetClusterCount();
							for (int j = 0; j < clusterCount; j++)
							{
								FbxCluster* cluster = newSkin->GetCluster(j);
								for (int k = 0; k < joints.size(); k++)
								{
									if (joints[k].node == cluster->GetLink())
									{
										int indexCount = cluster->GetControlPointIndicesCount();
										double* weights = cluster->GetControlPointWeights();
										int* indexArray = cluster->GetControlPointIndices();
										for (int l = 0; l < indexCount; l++)
										{
											end::influence influence_to_add{ k, (float)weights[l] };
											influence_set set = control_point_influences[indexArray[l]];

											int min = -1;
											for (int m = 0; m < MAX_INFLUENCES; m++)
											{
												if (set[m].weight < influence_to_add.weight)
												{
													if (min < 0)
														min = m;
													else if (set[m].weight < set[min].weight)
														min = m;
												}
											}
											if (min > -1)
											{
												set[min] = influence_to_add;
											}
											control_point_influences[indexArray[l]] = set;
										}
									}
								}
							}
						}
					}
				}

				end::skinned_mesh simpleMesh;

				// getting vertices and count from fbx
				simpleMesh.vert_count = mesh->GetControlPointsCount();
				simpleMesh.verts = new end::skinned_vert[simpleMesh.vert_count];
				for (uint32_t j = 0; j < simpleMesh.vert_count; j++)
				{
					simpleMesh.verts[j].pos = DirectX::XMFLOAT4{ (float)mesh->GetControlPointAt(j).mData[0], (float)mesh->GetControlPointAt(j).mData[1], (float)mesh->GetControlPointAt(j).mData[2], (float)mesh->GetControlPointAt(j).mData[3] };
				}
				// getting indices array and count from fbx
				simpleMesh.index_count = mesh->GetPolygonVertexCount();
				simpleMesh.indices = (uint32_t*)mesh->GetPolygonVertices();

				// getting uv's from fbx
				DirectX::XMFLOAT2 * UV = new DirectX::XMFLOAT2[simpleMesh.index_count];
				for (int j = 0; j < mesh->GetPolygonCount(); j++)//polygon(=mostly rectangle) count
				{
					FbxLayerElementArrayTemplate<FbxVector2>* uvVertices = NULL;
					mesh->GetTextureUV(&uvVertices);
					for (int k = 0; k < mesh->GetPolygonSize(j); k++)//retrieves number of vertices in a polygon
					{
						FbxVector2 uv = uvVertices->GetAt(mesh->GetTextureUVIndex(j, k));
						UV[3 * j + k] = DirectX::XMFLOAT2((float)uv.mData[0], 1 - (float)uv.mData[1]);
					}
				}

				// Get tangents and binormals
				FbxLayerElementArrayTemplate<FbxVector4> *fbxTangents;
				//FbxLayerElementArrayTemplate<FbxVector4> *fbxBinormal;
				mesh->GetTangents(&fbxTangents);
				//mesh->GetBinormals(&fbxBinormal);

				// getting normals from fbx
				FbxArray<FbxVector4> normalsVec;
				mesh->GetPolygonVertexNormals(normalsVec);

				end::skinned_vert* verts2 = new end::skinned_vert[simpleMesh.index_count];
				simpleMesh.vert_count = simpleMesh.index_count;
				// re-index
				for (uint32_t j = 0; j < simpleMesh.index_count; j++)
				{
					verts2[j] = simpleMesh.verts[simpleMesh.indices[j]];
					//XMStoreFloat4(&verts2[j].pos, XMVectorScale(XMLoadFloat4(&verts2[j].pos), 1/124.0f));
					//XMStoreFloat4(&verts2[j].pos, XMVectorScale(XMLoadFloat4(&verts2[j].pos), 3.25f));
					verts2[j].norm = DirectX::XMFLOAT3{ (float)normalsVec[j].mData[0], (float)normalsVec[j].mData[1], (float)normalsVec[j].mData[2] };
					verts2[j].tex = UV[j];

					influence_set set = control_point_influences[simpleMesh.indices[j]];

					verts2[j].indices = { (float)set[0].joint, (float)set[1].joint, (float)set[2].joint, (float)set[3].joint };

					DirectX::XMVECTOR normal{ set[0].weight, set[1].weight, set[2].weight, set[3].weight };
					normal = DirectX::XMVector4Normalize(normal);
					verts2[j].weights = { normal.m128_f32[0], normal.m128_f32[1], normal.m128_f32[2], normal.m128_f32[3] };

					verts2[j].tangents = { (float)fbxTangents->GetAt(j).mData[0], (float)fbxTangents->GetAt(j).mData[1], (float)fbxTangents->GetAt(j).mData[2], (float)fbxTangents->GetAt(j).mData[3] };
					//verts2[j].binormals = { (float)fbxBinormal->GetAt(j).mData[0], (float)fbxBinormal->GetAt(j).mData[1], (float)fbxBinormal->GetAt(j).mData[2], (float)fbxBinormal->GetAt(j).mData[3] };
				};

				delete[] simpleMesh.verts;
				simpleMesh.verts = verts2;

				unsigned int numIndices = 0;
				std::unordered_map<end::skinned_vert, unsigned int, end::Hash> uniqueValues;
				std::vector<end::skinned_vert> vertices;
				std::vector<unsigned int> indicesVector;

				for (uint32_t j = 0; j < simpleMesh.index_count; j++)
				{
					end::skinned_vert v = simpleMesh.verts[j];
					if (uniqueValues.count(v) == 0)
					{
						uniqueValues.insert({ v, numIndices });
						vertices.push_back(v);
						indicesVector.push_back(numIndices++);
					}
					else
						indicesVector.push_back(uniqueValues[v]);
				}

				std::ofstream file(output_file_path, std::ios_base::binary | std::ios_base::trunc);
				if (file.is_open())
				{
					int size = (int)vertices.size();
					file.write((const char*)&size, sizeof(int));
					file.write((const char*)vertices.data(), sizeof(end::skinned_vert) * vertices.size());
					size = (int)indicesVector.size();
					file.write((const char*)&size, sizeof(int));
					file.write((const char*)indicesVector.data(), sizeof(int) * indicesVector.size());
				}
				file.close();

				delete[] verts2;
				delete[] UV;
			}
		}
	}
	return 0;
}

FBXLOADING_API int export_bindpose(const char * fbx_file_path, const char * output_file_path, const char * mesh_name)
{

	int result = -1;
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);

	if (!scene)
		return result;
	FbxNode* childNode = nullptr;
	int childCount = scene->GetRootNode()->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		childNode = scene->GetRootNode()->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();
		if (mesh)
		{
			if (!mesh_name || mesh_name == mesh->GetName())
			{
				int posecount = scene->GetPoseCount();
				FbxPose* pose = scene->GetPose(0);
				FbxMesh* poseMesh = pose->GetNode(0)->GetMesh();
				std::vector<end::fbx_Joint> joints;
				if (pose->IsBindPose())
				{
					posecount = pose->GetCount();
					for (int i = 0; i < posecount; i++)
					{
						FbxSkeleton* skele = pose->GetNode(i)->GetSkeleton();
						if (skele && skele->IsSkeletonRoot())
						{
							end::fbx_Joint joint;
							joint.node = pose->GetNode(i);
							joint.parent_index = -1;
							joints.push_back(joint);
							for (int j = 0; j < joints.size(); j++)
							{
								for (int k = 0; k < joints[j].node->GetChildCount(); k++)
								{
									joint.node = joints[j].node->GetChild(k);
									joint.parent_index = j;
									joints.push_back(joint);
								}
							}
							break;
						}
					}
				}

				std::vector<DirectX::XMMATRIX> inversedBindMatrices;
				for (size_t i = 0; i < joints.size(); i++)
				{
					FbxAMatrix mat = joints[i].node->EvaluateGlobalTransform();
					XMMATRIX xmat;
					for (size_t j = 0; j < 4; j++)
						xmat.r[j] = { (float)mat.mData[j].mData[0], (float)mat.mData[j].mData[1], (float)mat.mData[j].mData[2], (float)mat.mData[j].mData[3] };
					
					//xmat = XMMatrixScaling(3.25f, 3.25f, 3.25f) * xmat;
					xmat = XMMatrixInverse(nullptr, xmat);
					inversedBindMatrices.push_back(xmat);
				}

				std::ofstream file(output_file_path, std::ios_base::binary | std::ios_base::trunc);
				if (file.is_open())
				{
					size_t size = inversedBindMatrices.size();
					file.write((const char*)&size, sizeof(size_t));
					file.write((const char*)inversedBindMatrices.data(), sizeof(DirectX::XMMATRIX) * inversedBindMatrices.size());
				}
				file.close();
				return 0;
			}
		}
	}
	return result;
}

FBXLOADING_API int export_animation(const char * fbx_file_path, const char * output_file_path, const char * mesh_name)
{
	int result = -1;
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);
	if (!scene)
		return result;
	FbxNode* childNode = nullptr;
	int childCount = scene->GetRootNode()->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		childNode = scene->GetRootNode()->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();
	
		if (mesh)
		{
			if (!mesh_name || mesh_name == mesh->GetName())
			{
				int posecount = scene->GetPoseCount();
				FbxPose* pose = scene->GetPose(0);
				FbxMesh* poseMesh = pose->GetNode(0)->GetMesh();
				std::vector<end::fbx_Joint> joints;
				if (pose->IsBindPose())
				{
					posecount = pose->GetCount();
					for (int i = 0; i < posecount; i++)
					{
						FbxSkeleton* skele = pose->GetNode(i)->GetSkeleton();


						if (skele)
						{
							if (skele->IsSkeletonRoot())
							{
								end::fbx_Joint joint;
								joint.node = pose->GetNode(i);
								joint.parent_index = -1;
								joints.push_back(joint);
								for (int j = 0; j < joints.size(); j++)
								{
									for (int k = 0; k < joints[j].node->GetChildCount(); k++)
									{
										skele = joints[j].node->GetChild(k)->GetSkeleton();
										if (skele)
										{
											joint.node = joints[j].node->GetChild(k);
											joint.parent_index = j;
											joints.push_back(joint);
										}
									}
								}
							}
						}
					}
				}

			
				FbxAnimStack* animStack = scene->GetCurrentAnimationStack();
				FbxTime duration = animStack->GetLocalTimeSpan().GetDuration();
				end::AnimClip clip;
				clip.duration = duration.GetSecondDouble();
				FbxTime::EMode framerate = FbxTime::EMode::eFrames30;
				FbxLongLong frames = duration.GetFrameCount(framerate);

				for (FbxLongLong i = 1; i <= frames; i++)
				{
					duration.SetFrame(i, framerate);
					end::KeyFrame frame;
					for (size_t j = 0; j < joints.size(); j++)
					{
						FbxAMatrix mat = joints[j].node->EvaluateGlobalTransform(duration);
						XMVECTOR vRot, vPos, vScale; XMMATRIX xmat;
						for (size_t j = 0; j < 4; j++)
							xmat.r[j] = { (float)mat.mData[j].mData[0], (float)mat.mData[j].mData[1], (float)mat.mData[j].mData[2], (float)mat.mData[j].mData[3] };
						
						XMMatrixDecompose(&vScale, &vRot, &vPos, xmat);

						end::Joint q;
						//vPos = XMVectorScale(vPos, 3.25f);
						q = { { vPos.m128_f32[0], vPos.m128_f32[1], vPos.m128_f32[2], vPos.m128_f32[3] }, { vRot.m128_f32[0], vRot.m128_f32[1], vRot.m128_f32[2], vRot.m128_f32[3] } };

						q.parent_index = joints[j].parent_index;

						frame.joints.push_back(q);
					}
					duration.SetFrame(i - 1, framerate);
					frame.time = duration.GetSecondDouble();
					clip.frames.push_back(frame);
				}

				std::ofstream file(output_file_path, std::ios_base::binary | std::ios_base::trunc);
				if (file.is_open())
				{
					size_t size;
					file.write((const char*)&clip.duration, sizeof(double));
					size = clip.frames.size();
					file.write((const char*)&size, sizeof(size_t));
					for (size_t i = 0; i < size; i++)
					{
						size_t temp = sizeof(end::Joint) * clip.frames[i].joints.size();
						file.write((const char*)&clip.frames[i].joints[0], temp);
						file.write((const char*)&clip.frames[i].time, sizeof(double));
					}

				}
				file.close();
				return 0;
			}
		}
	}
	return -1;
}

FBXLOADING_API void LoadAnimations(const char** pchFilepath, int nAnimationCount)
{

	std::string inputPath = std::string(pchFilepath[nAnimationCount]) + ".fbx";
	std::string outputPath = "..//Bin//" + std::string(pchFilepath[nAnimationCount + 1]) + "Data.bin";

	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(inputPath.c_str(), scene);

	if (!scene)
		return;
	FbxNode* childNode = nullptr;
	int childCount = scene->GetRootNode()->GetChildCount();
	std::vector<DirectX::XMMATRIX> BindPoseMatrices;
	for (int i = 0; i < childCount; i++)
	{
		childNode = scene->GetRootNode()->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();
		if (mesh)
		{
			//if (!mesh_name || mesh_name == mesh->GetName())
			{
				int posecount = scene->GetPoseCount();
				FbxPose* pose = scene->GetPose(0);
				FbxMesh* poseMesh = pose->GetNode(0)->GetMesh();
				std::vector<end::fbx_Joint> joints;
				if (pose->IsBindPose())
				{
					posecount = pose->GetCount();
					for (int i = 0; i < posecount; i++)
					{
						FbxSkeleton* skele = pose->GetNode(i)->GetSkeleton();
						if (skele && skele->IsSkeletonRoot())
						{
							end::fbx_Joint joint;
							joint.node = pose->GetNode(i);
							joint.parent_index = -1;
							joints.push_back(joint);
							for (int j = 0; j < joints.size(); j++)
							{
								for (int k = 0; k < joints[j].node->GetChildCount(); k++)
								{
									skele = joints[j].node->GetChild(k)->GetSkeleton();
									if (skele)
									{
										joint.node = joints[j].node->GetChild(k);
										joint.parent_index = j;
										joints.push_back(joint);
									}
								}
							}
							break;
						}
					}
				}

				for (size_t i = 0; i < joints.size(); i++)
				{
					FbxAMatrix mat = joints[i].node->EvaluateGlobalTransform();
					DirectX::XMMATRIX xmat;
					for (size_t j = 0; j < 4; j++)
					{
						xmat.r[j] = DirectX::XMVECTOR{ (float)mat.mData[j].mData[0], (float)mat.mData[j].mData[1], (float)mat.mData[j].mData[2], (float)mat.mData[j].mData[3] };
					}
					xmat = DirectX::XMMatrixInverse(nullptr, xmat);
					BindPoseMatrices.push_back(xmat);
				}
			}
		}
	}

	std::ofstream file(outputPath.c_str(), std::ios_base::binary | std::ios_base::trunc);
	if (file.is_open())
	{
		size_t size = BindPoseMatrices.size();
		file.write((const char*)&size, sizeof(size_t));
		file.write((const char*)BindPoseMatrices.data(), sizeof(DirectX::XMMATRIX) * BindPoseMatrices.size());
	}
	//file.close();

	int m_nAnimationCount = nAnimationCount;
	end::TAnimationClip* m_ptAnimationClips = new end::TAnimationClip[m_nAnimationCount];

	int MaxFrameCount = 0;
	for (int i = 0; i < m_nAnimationCount; i++)
	{
		std::fstream input;

		input.open(pchFilepath[i], std::ios_base::in | std::ios_base::binary);

		if (input.is_open())
		{
			// Duration
			input.read((char*)&m_ptAnimationClips[i].m_dDuration, sizeof(double));

			// Keyframe count
			input.read((char*)&m_ptAnimationClips[i].m_nKeyframeCount, sizeof(double));

			if (m_ptAnimationClips[i].m_nKeyframeCount > MaxFrameCount)
			{
				MaxFrameCount = m_ptAnimationClips[i].m_nKeyframeCount;
			}

			m_ptAnimationClips[i].m_ptKeyframes = new end::TKeyframe[m_ptAnimationClips[i].m_nKeyframeCount];

			for (int j = 0; j < m_ptAnimationClips[i].m_nKeyframeCount; j++)
			{
				// Joint

				m_ptAnimationClips[i].m_ptKeyframes[j].m_ptJoint = new end::TJoint[BindPoseMatrices.size()];


				input.read((char*)m_ptAnimationClips[i].m_ptKeyframes[j].m_ptJoint, sizeof(end::TJoint) * BindPoseMatrices.size());

				// Frame Time
				input.read((char*)&m_ptAnimationClips[i].m_ptKeyframes[j].m_dTime, sizeof(double));
			}

			input.close();
		}
	}

	for (size_t i = 0; i < m_nAnimationCount; i++)
	{
		end::TKeyframe * newFrame = new end::TKeyframe[MaxFrameCount];
		for (size_t j = 0; j < m_ptAnimationClips[i].m_nKeyframeCount; j++)
		{
			newFrame[j] = m_ptAnimationClips[i].m_ptKeyframes[j];
		}
		for (size_t j = m_ptAnimationClips[i].m_nKeyframeCount; j < MaxFrameCount; j++)
		{
			newFrame[j].m_ptJoint = new end::TJoint[BindPoseMatrices.size()]{ 0 };
			for (size_t k = 0; k < BindPoseMatrices.size(); k++)
			{
				newFrame[j].m_dTime = 0;
				newFrame[j].m_ptJoint[k] = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0 } };
			}
		}
		delete[] m_ptAnimationClips[i].m_ptKeyframes;
		m_ptAnimationClips[i].m_ptKeyframes = newFrame;
	}

	std::vector < DirectX::XMFLOAT4 > pos;
	std::vector < DirectX::XMFLOAT4 > rot;
	for (size_t i = 0; i < m_nAnimationCount; i++)
	{
		for (size_t j = 0; j < BindPoseMatrices.size(); j++)
		{
			for (size_t k = 0; k < MaxFrameCount; k++)
			{
				pos.push_back(m_ptAnimationClips[i].m_ptKeyframes[k].m_ptJoint[j].m_vPos);
				rot.push_back(m_ptAnimationClips[i].m_ptKeyframes[k].m_ptJoint[j].m_vRotation);
			}
		}
	}
	pos.push_back({ 0,0,0,0 });
	rot.push_back({ 0,0,0,0 });

	DirectX::Image AnimTexture;
	AnimTexture.format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	AnimTexture.width = MaxFrameCount;
	AnimTexture.height = (BindPoseMatrices.size() * m_nAnimationCount);
	DirectX::ComputePitch(AnimTexture.format, AnimTexture.width, AnimTexture.height, AnimTexture.rowPitch, AnimTexture.slicePitch);
	AnimTexture.pixels = (uint8_t*)pos.data();

	std::string strng = std::string(pchFilepath[m_nAnimationCount + 1]);
	std::wstring temp = std::wstring(strng.begin(), strng.end());
	std::wstring added = L"..//Bin//" + temp + L"PositionTexture.DDS";
	const wchar_t* sea = added.c_str();
	HRESULT hr = DirectX::SaveToDDSFile(AnimTexture, DirectX::DDS_FLAGS_NONE, sea);

	strng = std::string(pchFilepath[m_nAnimationCount + 1]);
	temp = std::wstring(strng.begin(), strng.end());
	added = L"..//Bin//" + temp + L"RotationTexture.DDS";
	sea = added.c_str();
	AnimTexture.pixels = (uint8_t*)rot.data();
	hr = DirectX::SaveToDDSFile(AnimTexture, DirectX::DDS_FLAGS_NONE, sea);


	//std::ofstream output(outputPath.c_str(), std::ios_base::binary | std::ios_base::app);
	//file.open();
	if (file.is_open())
	{
		file.write((const char*)&m_nAnimationCount, sizeof(m_nAnimationCount));
		for (int i = 0; i < nAnimationCount; i++)
		{
			file.write((const char*)&m_ptAnimationClips[i].m_dDuration, sizeof(m_ptAnimationClips[i].m_dDuration));
			file.write((const char*)&m_ptAnimationClips[i].m_nKeyframeCount, sizeof(m_ptAnimationClips[i].m_nKeyframeCount));
		}
	}
	file.close();

	for (size_t i = 0; i < m_nAnimationCount; i++)
	{
		for (size_t j = 0; j < MaxFrameCount; j++)
		{
			delete[] m_ptAnimationClips[i].m_ptKeyframes[j].m_ptJoint;
		}
		delete[] m_ptAnimationClips[i].m_ptKeyframes;
	}
	delete[] m_ptAnimationClips;
}

FBXLOADING_API int export_inversed_simple_mesh(const char * fbx_file_path, const char * output_file_path, const char * mesh_name)
{
	int result = -1;
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);

	if (!scene)
		return result;
	FbxNode* childNode = nullptr;
	int childCount = scene->GetRootNode()->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		childNode = scene->GetRootNode()->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();
		if (mesh)
		{
			if (!mesh_name || mesh_name == mesh->GetName())
			{
				end::simple_mesh simpleMesh;

				int colorCount = mesh->GetElementVertexColorCount();
				// getting vertices and count from fbx
				simpleMesh.vert_count = mesh->GetControlPointsCount();
				simpleMesh.verts = new end::simple_vert[simpleMesh.vert_count];
				for (uint32_t j = 0; j < simpleMesh.vert_count; j++)
				{
					simpleMesh.verts[j].pos = DirectX::XMFLOAT4{ (float)mesh->GetControlPointAt(j).mData[0], (float)mesh->GetControlPointAt(j).mData[1], (float)mesh->GetControlPointAt(j).mData[2], (float)mesh->GetControlPointAt(j).mData[3] };
				}
				// getting indices array and count from fbx
				simpleMesh.index_count = mesh->GetPolygonVertexCount();
				simpleMesh.indices = (uint32_t*)mesh->GetPolygonVertices();

				// getting uv's from fbx
				DirectX::XMFLOAT2 * UV = new DirectX::XMFLOAT2[simpleMesh.index_count];
				for (int j = 0; j < mesh->GetPolygonCount(); j++)//polygon(=mostly rectangle) count
				{
					FbxLayerElementArrayTemplate<FbxVector2>* uvVertices = NULL;
					mesh->GetTextureUV(&uvVertices);
					for (int k = 0; k < mesh->GetPolygonSize(j); k++)//retrieves number of vertices in a polygon
					{
						FbxVector2 uv = uvVertices->GetAt(mesh->GetTextureUVIndex(j, k));
						UV[3 * j + k] = DirectX::XMFLOAT2((float)uv.mData[0], 1 - (float)uv.mData[1]);
					}
				}
				// getting normals from fbx
				FbxArray<FbxVector4> normalsVec;
				mesh->GetPolygonVertexNormals(normalsVec);

				end::simple_vert* verts2 = new end::simple_vert[simpleMesh.index_count];
				simpleMesh.vert_count = simpleMesh.index_count;
				for (uint32_t j = 0; j < simpleMesh.index_count; j++)
				{
					verts2[j] = simpleMesh.verts[simpleMesh.indices[j]];
					verts2[j].norm = DirectX::XMFLOAT3{ (float)normalsVec[j].mData[0], (float)normalsVec[j].mData[1], (float)normalsVec[j].mData[2] };
					verts2[j].tex = UV[j];

					verts2[j].pos.z *= -1.0f;
					verts2[j].norm.z *= -1.0f;
				};

				delete[] simpleMesh.verts;
				simpleMesh.verts = verts2;

				unsigned int numIndices = 0;
				std::unordered_map<end::simple_vert, unsigned int, end::Hash> uniqueValues;
				std::vector<end::simple_vert> vertices;
				std::vector<unsigned int> indicesVector;

				for (uint32_t j = 0; j < simpleMesh.index_count; j++)
				{
					end::simple_vert v;
					v.norm = simpleMesh.verts[j].norm;
					v.pos = simpleMesh.verts[j].pos;
					v.tex = simpleMesh.verts[j].tex;

					if (uniqueValues.count(v) == 0)
					{
						uniqueValues.insert({ v, numIndices });
						vertices.push_back(v);
						indicesVector.push_back(numIndices++);
					}
					else
						indicesVector.push_back(uniqueValues[v]);
				}

				std::ofstream file(output_file_path, std::ios_base::binary | std::ios_base::trunc);
				if (file.is_open())
				{
					int size = (int)vertices.size();
					file.write((const char*)&size, sizeof(int));
					file.write((const char*)vertices.data(), sizeof(end::simple_vert) * size);
					size = (int)indicesVector.size();
					file.write((const char*)&size, sizeof(int));
					file.write((const char*)indicesVector.data(), sizeof(uint32_t) * size);
				}
				file.close();
				return 0;
			}
		}
	}
	return result;
}

FBXLOADING_API int export_inversed_skinned_mesh(const char * fbx_file_path, const char * output_file_path, const char * mesh_name)
{
	int result = -1;
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);

	if (!scene)
		return result;
	FbxNode* childNode = nullptr;
	int childCount = scene->GetRootNode()->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		childNode = scene->GetRootNode()->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();
		if (mesh)
		{
			if (!mesh_name || mesh_name == mesh->GetName())
			{
				const int MAX_INFLUENCES = 4;
				using influence_set = std::array<end::influence, MAX_INFLUENCES>;
				std::vector<influence_set> control_point_influences;
				control_point_influences.resize(mesh->GetControlPointsCount());

				int posecount = scene->GetPoseCount();
				FbxPose* pose = scene->GetPose(0);
				FbxMesh* poseMesh = pose->GetNode(0)->GetMesh();
				std::vector<end::fbx_Joint> joints;
				if (pose->IsBindPose())
				{
					posecount = pose->GetCount();
					for (int i = 0; i < posecount; i++)
					{
						FbxSkeleton* skele = pose->GetNode(i)->GetSkeleton();
						if (skele)
						{
							if (skele->IsSkeletonRoot())
							{
								end::fbx_Joint joint;
								joint.node = pose->GetNode(i);
								joint.parent_index = -1;
								joints.push_back(joint);
								for (int j = 0; j < joints.size(); j++)
								{
									for (int k = 0; k < joints[j].node->GetChildCount(); k++)
									{
										skele = joints[j].node->GetChild(k)->GetSkeleton();
										if (skele)
										{
											joint.node = joints[j].node->GetChild(k);
											joint.parent_index = j;
											joints.push_back(joint);
										}
									}
								}
							}
						}
					}

					int deformerCount = poseMesh->GetDeformerCount();
					for (int i = 0; i < deformerCount; i++)
					{
						FbxDeformer* skin = poseMesh->GetDeformer(i);
						if (skin->Is<FbxSkin>())
						{
							FbxSkin* newSkin = (FbxSkin*)skin;
							int clusterCount = newSkin->GetClusterCount();
							for (int j = 0; j < clusterCount; j++)
							{
								FbxCluster* cluster = newSkin->GetCluster(j);
								for (int k = 0; k < joints.size(); k++)
								{
									if (joints[k].node == cluster->GetLink())
									{
										int indexCount = cluster->GetControlPointIndicesCount();
										double* weights = cluster->GetControlPointWeights();
										int* indexArray = cluster->GetControlPointIndices();
										for (int l = 0; l < indexCount; l++)
										{
											end::influence influence_to_add{ k, (float)weights[l] };
											influence_set set = control_point_influences[indexArray[l]];

											int min = -1;
											for (int m = 0; m < MAX_INFLUENCES; m++)
											{
												if (set[m].weight < influence_to_add.weight)
												{
													if (min < 0)
														min = m;
													else if (set[m].weight < set[min].weight)
														min = m;
												}
											}
											if (min > -1)
											{
												set[min] = influence_to_add;
											}
											control_point_influences[indexArray[l]] = set;
										}
									}
								}
							}
						}
					}
				}

				end::skinned_mesh simpleMesh;

				// getting vertices and count from fbx
				simpleMesh.vert_count = mesh->GetControlPointsCount();
				simpleMesh.verts = new end::skinned_vert[simpleMesh.vert_count];
				for (uint32_t j = 0; j < simpleMesh.vert_count; j++)
				{
					simpleMesh.verts[j].pos = DirectX::XMFLOAT4{ (float)mesh->GetControlPointAt(j).mData[0], (float)mesh->GetControlPointAt(j).mData[1], (float)mesh->GetControlPointAt(j).mData[2], (float)mesh->GetControlPointAt(j).mData[3] };
				}
				// getting indices array and count from fbx
				simpleMesh.index_count = mesh->GetPolygonVertexCount();
				simpleMesh.indices = (uint32_t*)mesh->GetPolygonVertices();

				// getting uv's from fbx
				DirectX::XMFLOAT2 * UV = new DirectX::XMFLOAT2[simpleMesh.index_count];
				for (int j = 0; j < mesh->GetPolygonCount(); j++)//polygon(=mostly rectangle) count
				{
					FbxLayerElementArrayTemplate<FbxVector2>* uvVertices = NULL;
					mesh->GetTextureUV(&uvVertices);
					for (int k = 0; k < mesh->GetPolygonSize(j); k++)//retrieves number of vertices in a polygon
					{
						FbxVector2 uv = uvVertices->GetAt(mesh->GetTextureUVIndex(j, k));
						UV[3 * j + k] = DirectX::XMFLOAT2((float)uv.mData[0], 1 - (float)uv.mData[1]);
					}
				}
				// getting normals from fbx
				FbxArray<FbxVector4> normalsVec;
				mesh->GetPolygonVertexNormals(normalsVec);

				end::skinned_vert* verts2 = new end::skinned_vert[simpleMesh.index_count];
				simpleMesh.vert_count = simpleMesh.index_count;
				// re-index
				for (uint32_t j = 0; j < simpleMesh.index_count; j++)
				{
					verts2[j] = simpleMesh.verts[simpleMesh.indices[j]];
					//verts2[j].pos.z *= -1.0f;
					verts2[j].norm = DirectX::XMFLOAT3{ (float)normalsVec[j].mData[0], (float)normalsVec[j].mData[1], (float)normalsVec[j].mData[2] * -1.0f };
					verts2[j].tex = UV[j];

					influence_set set = control_point_influences[simpleMesh.indices[j]];

					verts2[j].indices.x = (float)set[0].joint;
					verts2[j].indices.y = (float)set[1].joint;
					verts2[j].indices.z = (float)set[2].joint;
					verts2[j].indices.w = (float)set[3].joint;

					DirectX::XMVECTOR normal{ set[0].weight, set[1].weight, set[2].weight, set[3].weight };
					normal = DirectX::XMVector4Normalize(normal);
					verts2[j].weights.x = normal.m128_f32[0];
					verts2[j].weights.y = normal.m128_f32[1];
					verts2[j].weights.z = normal.m128_f32[2];
					verts2[j].weights.w = normal.m128_f32[3];

				};

				delete[] simpleMesh.verts;
				simpleMesh.verts = verts2;

				unsigned int numIndices = 0;
				std::unordered_map<end::skinned_vert, unsigned int, end::Hash> uniqueValues;
				std::vector<end::skinned_vert> vertices;
				std::vector<unsigned int> indicesVector;

				for (uint32_t j = 0; j < simpleMesh.index_count; j++)
				{
					end::skinned_vert v = simpleMesh.verts[j];

					if (uniqueValues.count(v) == 0)
					{
						uniqueValues.insert({ v, numIndices });
						vertices.push_back(v);
						indicesVector.push_back(numIndices++);
					}
					else
						indicesVector.push_back(uniqueValues[v]);
				}

				std::ofstream file(output_file_path, std::ios_base::binary | std::ios_base::trunc);
				if (file.is_open())
				{
					int size = (int)vertices.size();
					file.write((const char*)&size, sizeof(int));
					file.write((const char*)vertices.data(), sizeof(end::skinned_vert) * vertices.size());
					size = (int)indicesVector.size();
					file.write((const char*)&size, sizeof(int));
					file.write((const char*)indicesVector.data(), sizeof(int) * indicesVector.size());
				}
				file.close();
			}
		}
	}
	return 0;
}

FBXLOADING_API int export_inversed_bindpose(const char * fbx_file_path, const char * output_file_path, const char * mesh_name)
{
	int result = -1;
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);

	if (!scene)
		return result;
	FbxNode* childNode = nullptr;
	int childCount = scene->GetRootNode()->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		childNode = scene->GetRootNode()->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();
		if (mesh)
		{
			if (!mesh_name || mesh_name == mesh->GetName())
			{
				int posecount = scene->GetPoseCount();
				FbxPose* pose = scene->GetPose(0);
				FbxMesh* poseMesh = pose->GetNode(0)->GetMesh();
				std::vector<end::fbx_Joint> joints;
				if (pose->IsBindPose())
				{
					posecount = pose->GetCount();
					for (int i = 0; i < posecount; i++)
					{
						FbxSkeleton* skele = pose->GetNode(i)->GetSkeleton();
						if (skele && skele->IsSkeletonRoot())
						{
							end::fbx_Joint joint;
							joint.node = pose->GetNode(i);
							joint.parent_index = -1;
							joints.push_back(joint);
							for (int j = 0; j < joints.size(); j++)
							{
								for (int k = 0; k < joints[j].node->GetChildCount(); k++)
								{
									skele = joints[j].node->GetChild(k)->GetSkeleton();
									if (skele)
									{
										joint.node = joints[j].node->GetChild(k);
										joint.parent_index = j;
										joints.push_back(joint);
									}
								}
							}
							break;
						}
					}
				}

				std::vector<DirectX::XMMATRIX> newQs;
				for (size_t i = 0; i < joints.size(); i++)
				{
					FbxAMatrix mat = joints[i].node->EvaluateGlobalTransform();
					DirectX::XMMATRIX xmat;
					for (size_t j = 0; j < 4; j++)
					{
						xmat.r[j] = DirectX::XMVECTOR{ (float)mat.mData[j].mData[0], (float)mat.mData[j].mData[1], (float)mat.mData[j].mData[2] * -1.0f , (float)mat.mData[j].mData[3] };
					}
					xmat.r[3].m128_f32[3] *= -1.0f;
					xmat = DirectX::XMMatrixInverse(nullptr, xmat);
					newQs.push_back(xmat);
				}

				std::ofstream file(output_file_path, std::ios_base::binary | std::ios_base::trunc);
				if (file.is_open())
				{
					size_t size = newQs.size();
					file.write((const char*)&size, sizeof(size_t));
					file.write((const char*)newQs.data(), sizeof(DirectX::XMMATRIX) * newQs.size());
					size = 1;
					file.write((const char*)&size, sizeof(size_t));
				}
				file.close();
				return 0;
			}
		}
	}
	return result;
}

FBXLOADING_API int export_inversed_animation(const char * fbx_file_path, const char * output_file_path, const char * mesh_name)
{
	int result = -1;
	FbxScene* scene = nullptr;
	FbxManager* sdk_manager = create_and_import(fbx_file_path, scene);

	if (!scene)
		return result;
	FbxNode* childNode = nullptr;
	int childCount = scene->GetRootNode()->GetChildCount();
	for (int i = 0; i < childCount; i++)
	{
		childNode = scene->GetRootNode()->GetChild(i);
		FbxMesh* mesh = childNode->GetMesh();
		if (mesh)
		{
			if (!mesh_name || mesh_name == mesh->GetName())
			{
				int posecount = scene->GetPoseCount();
				FbxPose* pose = scene->GetPose(0);
				FbxMesh* poseMesh = pose->GetNode(0)->GetMesh();
				std::vector<end::fbx_Joint> joints;
				if (pose->IsBindPose())
				{
					posecount = pose->GetCount();
					for (int i = 0; i < posecount; i++)
					{
						FbxSkeleton* skele = pose->GetNode(i)->GetSkeleton();
						if (skele && skele->IsSkeletonRoot())
						{
							end::fbx_Joint joint;
							joint.node = pose->GetNode(i);
							joint.parent_index = -1;
							joints.push_back(joint);
							for (int j = 0; j < joints.size(); j++)
							{
								for (int k = 0; k < joints[j].node->GetChildCount(); k++)
								{
									skele = joints[j].node->GetChild(k)->GetSkeleton();
									if (skele)
									{
										joint.node = joints[j].node->GetChild(k);
										joint.parent_index = j;
										joints.push_back(joint);
									}
								}
							}
							break;
						}
					}
				}

				FbxAnimStack* animStack = scene->GetCurrentAnimationStack();
				FbxTime duration = animStack->GetLocalTimeSpan().GetDuration();
				end::AnimClip clip;
				clip.duration = duration.GetSecondDouble();
				FbxLongLong frames = duration.GetFrameCount(FbxTime::EMode::eFrames24);

				for (FbxLongLong i = 1; i < frames; i++)
				{
					duration.SetFrame(i, FbxTime::EMode::eFrames24);
					end::KeyFrame frame;
					frame.time = duration.GetSecondDouble();
					for (size_t j = 0; j < joints.size(); j++)
					{
						FbxAMatrix mat = joints[j].node->EvaluateGlobalTransform(duration);
						/*for (size_t k = 0; k < 4; k++)
						{
							mat.mData[k].mData[3] *= -1.0f;
						}
						mat.mData[3].mData[3] *= -1.0f;*/
						end::Joint q;
						q.form = DirectX::XMFLOAT4{ (float)mat.GetQ().mData[0], (float)mat.GetQ().mData[1], (float)mat.GetQ().mData[2] * -1.0f , (float)mat.GetQ().mData[3] };;
						q.pos = DirectX::XMFLOAT4{ (float)mat.mData[3].mData[0], (float)mat.mData[3].mData[1], (float)mat.mData[3].mData[2] * -1.0f, (float)mat.mData[3].mData[3] };
						q.parent_index = joints[j].parent_index;

						frame.joints.push_back(q);
					}
					clip.frames.push_back(frame);
				}

				std::ofstream file(output_file_path, std::ios_base::binary | std::ios_base::trunc);
				if (file.is_open())
				{
					size_t size;
					file.write((const char*)&clip.duration, sizeof(double));
					size = clip.frames.size();
					file.write((const char*)&size, sizeof(size_t));
					for (size_t i = 0; i < size; i++)
					{
						size_t temp = sizeof(end::Joint) * clip.frames[i].joints.size();
						file.write((const char*)&clip.frames[i].joints[0], temp);
						file.write((const char*)&clip.frames[i].time, sizeof(double));
					}

				}
				file.close();
				return 0;
			}
		}
	}
	return -1;
}
