#include "cMaterial.h"

cMaterial::cMaterial(Shader* inShader, std::vector<sTexture> inTextures)
	:MaterialShader(inShader),
	Textures(inTextures)
{

}

