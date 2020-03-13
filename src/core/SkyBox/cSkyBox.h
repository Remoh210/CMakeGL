#pragma once 

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <vector>

class cSkyBox
{
public:

	//Order = right, left, top, bottom, front, back
	cSkyBox(std::vector<std::string> &Textures, const char* skyBoxVS, const char* skyBoxFS, Camera* camera);
	~cSkyBox();

	//!! draw skybox as last!!
	void Draw(glm::mat4 view, glm::mat4 projection, GLuint& depthTex);

	inline unsigned int GetTextureID() { return TextureID; }
	inline Shader* GetShader() { return SkyBoxShader; }

	
private:
	std::vector<std::string> VecTextures;
	Shader* SkyBoxShader;
	unsigned int TextureID;
	unsigned int SkyboxVAO;
	unsigned int SkyboxVBO;
	Camera* MainCamera;

};

