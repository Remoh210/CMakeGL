#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../../thirdParty/imgui/imgui.h"
#include "../../thirdParty/imgui/imgui_impl_glfw.h"
#include "../../thirdParty/imgui/imgui_impl_opengl3.h"

#include "../fbo/cFBO.h"

class UI //Make it static class?
{
private:
    ImVec2 WindowSize;
    ImVec2 SceneViewSize;
    float SceneViewScale;
    /* data */
    cFBO* SceneFBO;


    void DrawSceneEditor();
//Draw properties
//etc

public:
    UI(ImVec2 window_size, ImVec2 scene_view_size, cFBO* scene_fbo, GLFWwindow *window);
    ~UI();

    void DrawUI();
    void RenderUI();
    void ResizeUI(int width, int height);

    inline void SetSceneScale(float scale) { SceneViewScale = scale; };
    inline float GetSceneScale(){ return SceneViewScale; }

};

