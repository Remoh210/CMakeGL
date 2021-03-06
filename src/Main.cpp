#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/UI/UI.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

#include "core/fbo/cFBO.h"
#include "core/SkyBox/cSkyBox.h"
#include "core/SkyBox/cSkyBox.h"


#include "core/AssetImporter/cAssetImporter.h"

void window_size_callback(GLFWwindow* window, int width, int height);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void ResizeFBOs(int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
unsigned int SCR_WIDTH = 1000;
unsigned int SCR_HEIGHT = 600;

float SceneViewScale = 0.65;
int SceneViewHeight = SCR_HEIGHT * SceneViewScale;
int SceneViewWidth = SCR_WIDTH * SceneViewScale;

//AssetLoader
cAssetImporter* AssetLoader = nullptr;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 8.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//UI
UI* EditorUI = nullptr;

//FBO
cFBO* GBuffer = nullptr;
cFBO* SceneViewFBO = nullptr;
cFBO* MotionBlurFBO = nullptr;
//Put in a seopareta Class
unsigned int quadVAO = 0;
unsigned int quadVBO;

glm::mat4 OldVP(1.f); //For MotionBlur

void renderQuad()
{
	if (quadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Editor", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



	// configure global opengl state
	// -----------------------------
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	// -------------------------
	Shader* shaderGeometryPass = new Shader(FileSystem::getPath("resources/shaders/main_pipeline/g_buffer.vs").c_str(), FileSystem::getPath("resources/shaders/main_pipeline/g_buffer.fs").c_str());
	Shader shaderLightingPass(FileSystem::getPath("resources/shaders/main_pipeline/lighting_pass.vs").c_str(), FileSystem::getPath("resources/shaders/main_pipeline/lighting_pass.fs").c_str());
	//Shader shaderLightBox("8.1.deferred_light_box.vs", "8.1.deferred_light_box.fs");
	Shader motionBlurPass(FileSystem::getPath("resources/shaders/main_pipeline/motion_blur.vs").c_str(), FileSystem::getPath("resources/shaders/main_pipeline/motion_blur.fs").c_str());

    // load models
    // -----------
	AssetLoader = new cAssetImporter(shaderGeometryPass);
	
	AssetLoader->LoadModel(FileSystem::getPath("resources/objects/mutant/FBX 2013/mutant.fbx"));
	//AssetLoader->LoadModel(FileSystem::getPath("resources/objects/cyborg/cyborg.obj"));
	//AssetLoader->LoadModel(FileSystem::getPath("resources/objects/nanosuit/nanosuit.obj"));
	//AssetLoader->LoadModel(FileSystem::getPath("resources/objects/cube/cube.fbx"));

	std::vector<glm::vec3> objectPositions;

	objectPositions.push_back(glm::vec3(-3.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, -3.0));
	objectPositions.push_back(glm::vec3(-3.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, 0.0));
	objectPositions.push_back(glm::vec3(-3.0, -3.0, 3.0));
	objectPositions.push_back(glm::vec3(0.0, -3.0, 3.0));
	objectPositions.push_back(glm::vec3(3.0, -3.0, 3.0));

	//SetUp Fbos
	GBuffer = new cFBO();
	std::string FboErr;
	GBuffer->init(SceneViewWidth, SceneViewHeight, FboErr);

	SceneViewFBO = new cFBO();
	SceneViewFBO->init(SceneViewWidth, SceneViewHeight, FboErr);

    MotionBlurFBO = new cFBO();
    MotionBlurFBO->init(SceneViewWidth, SceneViewHeight, FboErr);

	// lighting info
	// -------------
	const unsigned int NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		float yPos = ((rand() % 100) / 100.0) * 6.0 - 4.0;
		float zPos = ((rand() % 100) / 100.0) * 6.0 - 3.0;
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}


	shaderLightingPass.use();
	shaderLightingPass.setInt("gPosition", 0);
	shaderLightingPass.setInt("gNormal", 1);
	shaderLightingPass.setInt("gAlbedoSpec", 2);
	shaderLightingPass.setInt("gDTex", 3);

	motionBlurPass.use();
    motionBlurPass.setInt("gRenderedTex", 0);
	motionBlurPass.setInt("gDepthTex", 1);

    EditorUI = new UI(ImVec2(SCR_WIDTH, SCR_HEIGHT), ImVec2(SceneViewWidth, SceneViewHeight), SceneViewFBO, window, ResizeFBOs);
	EditorUI->GBuffer = GBuffer;
	EditorUI->AssetImporter = AssetLoader;

	std::vector<std::string> VecSkyboxTex
	{
		FileSystem::getPath("resources/textures/skybox/right.jpg"),
		FileSystem::getPath("resources/textures/skybox/left.jpg"),
		FileSystem::getPath("resources/textures/skybox/top.jpg"),
		FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
		FileSystem::getPath("resources/textures/skybox/front.jpg"),
		FileSystem::getPath("resources/textures/skybox/back.jpg")
	};

	cSkyBox SkyBox(VecSkyboxTex, "sky_box.vs", "sky_box.fs", &camera);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        EditorUI->DrawUI();
        
		//Geometry pass
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 1. geometry pass: render scene's geometry/color data into gbuffer
		// -----------------------------------------------------------------
		GBuffer->bindBuffer();
		glViewport(0, 0, EditorUI->GetSceneViewSize().x, EditorUI->GetSceneViewSize().y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)EditorUI->GetSceneViewSize().x / (float)EditorUI->GetSceneViewSize().y, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
		//shaderGeometryPass.use();
		//shaderGeometryPass.setMat4("projection", projection);

		//shaderGeometryPass.setMat4("view", view);
		glm::mat4 VP = projection * view;
		for (unsigned int i = 0; i < objectPositions.size(); i++)
		{
			model = glm::mat4(1.0f);
			model = glm::translate(model, objectPositions[i]);
			model = glm::scale(model, glm::vec3(0.015f));

			glm::mat4 MVP = VP * model;
			//shaderGeometryPass.setMat4("MVP", MVP);
			//shaderGeometryPass.setMat4("model", model);
			AssetLoader->vec_static_mesh[0]->Draw(model, view, projection, MVP);
			//nanosuit.Draw(shaderGeometryPass);
		}
		
		//Draw Skybox
		SkyBox.Draw(view, projection, GBuffer->depthTexture_ID);

		// lighting pass
		SceneViewFBO->bindBuffer();

		glViewport(0, 0, EditorUI->GetSceneViewSize().x, EditorUI->GetSceneViewSize().y);

		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderLightingPass.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, GBuffer->vertexWorldPos_2_ID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GBuffer->normalTexture_1_ID);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, GBuffer->colourTexture_0_ID);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, GBuffer->depthTexture_ID);

		for (unsigned int i = 0; i < lightPositions.size(); i++)
		{
			shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
			shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
			// update attenuation parameters and calculate radius
			const float constant = 1.0; // note that we don't send this to the shader, we assume it is always 1.0 (in our case)
			const float linear = 0.2;
			const float quadratic = 1.2;
			shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
			shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
		}
		shaderLightingPass.setVec3("viewPos", camera.Position);

		renderQuad();

        MotionBlurFBO->bindBuffer();
		glViewport(0, 0, EditorUI->GetSceneViewSize().x, EditorUI->GetSceneViewSize().y);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Motion Blur Pass
        motionBlurPass.use();

		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, SceneViewFBO->colourTexture_0_ID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, GBuffer->depthTexture_ID);

        motionBlurPass.setMat4("VP", VP);
        motionBlurPass.setMat4("PreviousVP", OldVP);
		OldVP = VP;
		motionBlurPass.setInt("BlurCycleCount", EditorUI->GetPostProcessingSettings().BlurCycles);
		motionBlurPass.setFloat("VelocityMult", EditorUI->GetPostProcessingSettings().PixelVelocityMult);

        renderQuad();

		
		MotionBlurFBO->unbindBuffer();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //UI->Render
        EditorUI->RenderUI();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
    {
        camera.bActive = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        camera.bActive = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    //glViewport(0, 0, width/2, height/2);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;

    SceneViewHeight = SCR_HEIGHT * EditorUI->GetSceneScale();
    SceneViewWidth = SCR_WIDTH * EditorUI->GetSceneScale();

    EditorUI->ResizeUI(SCR_WIDTH, SCR_HEIGHT);

	ResizeFBOs(SceneViewWidth, SceneViewHeight);

}

void ResizeFBOs(int width, int height)
{
	std::string err = "";
	GBuffer->reset(width, height, err);
	if (!err.empty())
	{
		std::printf(err.c_str());
		err.clear();
	}
	SceneViewFBO->reset(width, height, err);
	if (!err.empty())
	{
		std::printf(err.c_str());
		err.clear();
	}
	MotionBlurFBO->reset(width, height, err);
	if (!err.empty())
	{
		std::printf(err.c_str());
		err.clear();
	}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.

}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
