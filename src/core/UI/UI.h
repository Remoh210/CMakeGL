#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../thirdParty/imgui/imgui.h"
#include "../../thirdParty/imgui/imgui_impl_glfw.h"
#include "../../thirdParty/imgui/imgui_impl_opengl3.h"

#include "../fbo/cFBO.h"

typedef void(*FBOResizeFunc)(int, int);

class UI //Make it static class?
{
private:
    void DrawSceneEditor();

	ImVec2 WindowSize;
	ImVec2 SceneViewSize;
	float SceneViewScale;
	cFBO* SceneFBO;
	float SceneViewScaleCache;
	GLFWwindow* Window;

	FBOResizeFunc ResizeCallbackFunc;

	bool bFullScreen;

	void ToggleFullscreen();

public:
    UI(ImVec2 window_size, ImVec2 scene_view_size, cFBO* scene_fbo, GLFWwindow* window, FBOResizeFunc resize_callback);
    ~UI();

    void DrawUI();
    void ResizeUI(int width, int height);

    inline void SetSceneScale(float scale) { SceneViewScale = scale; };
    inline float GetSceneScale(){ return SceneViewScale; }
	inline ImVec2 GetSceneViewSize() { return SceneViewSize; }
	
	void RenderUI();
};

