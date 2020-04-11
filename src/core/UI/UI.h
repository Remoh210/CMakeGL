#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../thirdParty/imgui/imgui.h"
#include "../../thirdParty/imgui/imgui_impl_glfw.h"
#include "../../thirdParty/imgui/imgui_impl_opengl3.h"
#include "../AssetImporter/cAssetImporter.h"

#include "../fbo/cFBO.h"

typedef void(*FBOResizeFunc)(int, int);


struct sPostProcessingSettings
{
	sPostProcessingSettings()
	{
		BlurCycles = 10;
		PixelVelocityMult = 0.1;
	}

	float PixelVelocityMult;
	int BlurCycles;
};

class UI //Make it static class?
{
private:
    void DrawMainBar();
    void DrawSceneEditor();
    void DrawSceneTree();
    void DrawInspector();
	void DrawSettings();
    //void Draw();



	ImVec2 WindowSize;
	ImVec2 SceneViewSize;
	float SceneViewScale;
	cFBO* SceneFBO;
	float SceneViewScaleCache;
	GLFWwindow* Window;

	FBOResizeFunc ResizeCallbackFunc;

	bool bFullScreen;
	bool bShowSettings;

	void ToggleFullscreen();
	void ToggleSettingsWindow();

	void HelpMarker(const char* desc);

	sPostProcessingSettings PostProcessingSettings;

public:
    UI(ImVec2 window_size, ImVec2 scene_view_size, cFBO* scene_fbo, GLFWwindow* window, FBOResizeFunc resize_callback);
    ~UI();
	
	//Test
	cAssetImporter* AssetImporter;
	cFBO* GBuffer;
	//Test

    void DrawUI();
    void ResizeUI(int width, int height);

    inline void SetSceneScale(float scale) { SceneViewScale = scale; };
    inline float GetSceneScale(){ return SceneViewScale; }
	inline ImVec2 GetSceneViewSize() { return SceneViewSize; }
	
	inline sPostProcessingSettings GetPostProcessingSettings() { return PostProcessingSettings; }

	void RenderUI();
};

