#pragma once

//common
#include<string>
#include<vector>
//common

#include <learnopengl/shader.h>
#include "sTexture.h"

//
// Material class
//
class cMaterial
{
public:

    cMaterial(Shader* inShader, std::vector<sTexture> inTextures);
	~cMaterial();

	Shader* MaterialShader;
	std::vector<sTexture> Textures;
};


