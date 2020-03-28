//
#include <map>
//

#pragma once

#include "../Material/cMaterial.h"
#include "../Material/sTexture.h"
#include "../Mesh/cStaticMesh.h"


#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <learnopengl/shader.h>

class cAssetImporter
{
public:
    cAssetImporter();
    void LoadModel(std::string const &path);
	unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false);
	std::vector<cStaticMesh*> vec_static_mesh;


private:

	void processNode(aiNode *node, const aiScene *scene);
	cMesh* processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<sTexture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);

	std::string mDirectory;
	std::vector<sTexture> mLoadedTextures;

	//??
	std::map<std::string, Shader*> mMainShaders;

	//
	std::vector<cMesh*> vec_mesh_temp;
};
