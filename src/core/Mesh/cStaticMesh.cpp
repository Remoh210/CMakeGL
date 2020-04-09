#include "cStaticMesh.h"

cStaticMesh::cStaticMesh(std::vector<cMesh*> vec_mesh)
{
	mMeshes = vec_mesh;
}

void cStaticMesh::cStaticMesh::Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::mat4 MVP)
{

	for (unsigned int i = 0; i < mMeshes.size(); i++)
		mMeshes[i]->Draw(model, view, projection, MVP);
}

