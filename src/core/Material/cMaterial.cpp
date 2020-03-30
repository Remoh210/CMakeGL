#include "cMaterial.h"

cMaterial::cMaterial(Shader* inShader, std::vector<sTexture*> inTextures)
	:MaterialShader(inShader),
	Textures(inTextures)
{
	DiffuseColour = glm::vec4(1.f, 1.f, 1.f, 1.f);
}