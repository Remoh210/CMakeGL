#include "cSkyBox.h"
#include <stb_image.h>

float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

cSkyBox::cSkyBox(std::vector<std::string> &Textures, const char* skyBoxVS, const char* skyBoxFS, Camera* camera)
		:VecTextures(Textures),
		 MainCamera(camera)
{
	SkyBoxShader = new Shader(skyBoxVS, skyBoxFS);
	if (!SkyBoxShader)
	{
		std::cout << "SkyBoxShader is null" << std::endl; //Log
	}

	SkyBoxShader->use();
	SkyBoxShader->setInt("skybox", 0);

	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < Textures.size(); i++)
	{
		unsigned char *data = stbi_load(Textures[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << Textures[i] << std::endl; //TODO: Log
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	// skybox VAO
	
	glGenVertexArrays(1, &SkyboxVAO);
	glGenBuffers(1, &SkyboxVBO);
	glBindVertexArray(SkyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, SkyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

}

cSkyBox::~cSkyBox()
{
	//RemoveTextures
}

void cSkyBox::Draw(glm::mat4 view, glm::mat4 projection, GLuint& depthTex)
{

	glDepthMask(GL_FALSE);
	SkyBoxShader->use();
	glm::mat4 NewView = glm::mat4(glm::mat3(view)); // remove translation from the view matrix
	SkyBoxShader->setMat4("invView", glm::inverse(view));
	SkyBoxShader->setMat4("invProj", glm::inverse(projection));
	// skybox cube
	glBindVertexArray(SkyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, TextureID);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDepthMask(GL_TRUE);
	//glDepthRange(0.0, 1.0);
}
