#include "UI.h"

UI::UI(ImVec2 window_size, ImVec2 scene_view_size, cFBO* scene_fbo, GLFWwindow *window)
        :WindowSize(window_size),
         SceneViewSize(scene_view_size),
         SceneFBO(scene_fbo)
{

    SceneViewScale = 0.65;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    // draw in wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    ImGui::StyleColorsDark();
}

UI::~UI()
{
}

void UI::DrawUI()
{
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    DrawSceneEditor();

}

void UI::DrawSceneEditor()
{
    ImVec2 VecScreen = SceneViewSize;
    VecScreen.y += 20;
    ImGui::SetNextWindowSize(VecScreen);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.0f);

    ImGui::Begin("Scene View");
    ImGui::Image((void*)SceneFBO->colourTexture_0_ID, SceneViewSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();



    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
}

void UI::RenderUI()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::ResizeUI(int width, int height)
{
    int NewWidth = width * SceneViewScale;
    int NewHeight = height * SceneViewScale;
    SceneViewSize = ImVec2(NewWidth, NewHeight);
}
