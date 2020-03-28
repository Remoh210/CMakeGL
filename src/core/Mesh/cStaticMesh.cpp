#include "cStaticMesh.h"

cStaticMesh::cStaticMesh(std::vector<cMesh*> vec_mesh)
{
	mMeshes = vec_mesh;
}

void cStaticMesh::cStaticMesh::Draw()
{
	for (unsigned int i = 0; i < mMeshes.size(); i++)
		mMeshes[i]->Draw();
}

