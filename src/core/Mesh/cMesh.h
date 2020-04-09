#pragma once

//Common
#include <vector>
#include "sVertex.h"
#include "../Material/cMaterial.h"

class cMesh
{

public:
	cMesh(std::vector<sVertex> vertices, std::vector<unsigned int> indices, cMaterial* material);
	~cMesh();

	// render the mesh
	void Draw(glm::mat4 model, glm::mat4 view, glm::mat4 projection, glm::mat4 MVP);

	/*  Mesh Data  */
	unsigned int VAO;
	std::vector<sVertex> mVertices;
	std::vector<unsigned int> mIndices;

	cMaterial* mMaterial;

private:

	/*  Render data  */
	unsigned int VBO, EBO;

	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh();

};