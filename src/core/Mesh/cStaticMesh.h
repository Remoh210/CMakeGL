#pragma once

//Common
#include <vector>
#include "cMesh.h"

class cStaticMesh
{
public:
	cStaticMesh(std::vector<cMesh*> vec_mesh);
	~cStaticMesh();

	// draws the model, and thus all its meshes
	void Draw();

private:
	std::vector<cMesh*> mMeshes;
};