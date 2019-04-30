#pragma once

// Example library interface
// Your library will wrap usage of the FbxSDK

// NOTE:
// You are not required to use the provided types and prototypes but you must have functional parity.
// You MUST ensure that your client application does not require any knowledge of Fbx headers, libraries, or types.
//
// Tip:
// Keep interface and implementation seperate.
// 'export_simple_mesh' or your own version should be the only function that outside applications need to see.
// You should not need to #include <fbxsdk.h> anywhere but your cpp file.
// You should not need to have your function/prototype for the "Mesh Processing" step exported or included anywhere.

namespace end
{
	// export_simple_mesh
	//
	// Performs the followings steps :
	// 
	// *Fbx Setup and scene loading 
	//	1.Initialize the SDK manager.
	//	2.Create the IO settings object.
	//	3.Create an importer using the SDK manager.
	//	4.Use the first argument as the filename for the importer.
	//	5.Create a new scene so that it can be populated by the imported file.
	//	6.Import the contents of the file into the scene.
	//	7.The file is imported; so get rid of the importer.
	//	
	//	8.Invokes functionality specified below in "Mesh Processing"
	//	
	//	9.Destroy the SDK manager and all the other objects it was handling.
	//	10.Returns 0 on success, non-zero to indicate failure
	//
	// *Mesh processing - Calls a function YOU DEFINE that implements the following steps
	//	1.Searches the scene for a mesh with 'mesh_name'
	//		a. If 'mesh_name' is null, proceed by using the first mesh in the scene
	//		b. if no match is found, return an integer error code (ex: -1)
	//	2.Extract vertex data from the mesh and store the data in a 'simple_mesh' object or similar
	//		a. SEE "EXPORTING GUIDE.PDF" for pseudocode and in-depth explaination
	//		a. See SDK example code VBOMesh::Initialize() in <FbxSDK Directory>\Samples\ViewScene\SceneCache.cxx
	//	3.Write the 'simple_mesh' object data to a binary file using 'output_file_path'
	int export_simple_mesh(const char* fbx_file_path, const char* output_file_path, const char* mesh_name = nullptr);
}