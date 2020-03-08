#include "UI.h"
#include <iostream>


UI::UI(ImVec2 window_size, ImVec2 scene_view_size, cFBO* scene_fbo, GLFWwindow* window, FBOResizeFunc resize_callback)
        :WindowSize(window_size),
		SceneViewSize(scene_view_size),
        SceneFBO(scene_fbo),
		Window(window),
		ResizeCallbackFunc(resize_callback)
		
{

    SceneViewScale = 0.65;
	bFullScreen = false;

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
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoResize;


	ImVec2 VecScreen = SceneViewSize;
	int VerticalSizeAdj = 45;
	if (bFullScreen)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		VerticalSizeAdj = 20;
	}
		
	VecScreen.y += VerticalSizeAdj;

	ImGui::SetNextWindowSize(VecScreen);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.0f);

    ImGui::Begin("Scene View", nullptr, window_flags);

	// Menu Bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::MenuItem("Save");
			ImGui::MenuItem("Load");
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Full Screen", "Ctrl + F"))
			{
				ToggleFullscreen();
			}
			
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
		

    ImGui::Image((void*)SceneFBO->colourTexture_0_ID, SceneViewSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();



    ImGui::PopStyleVar();
    ImGui::PopStyleVar();

	ImGui::ShowDemoWindow();
}

void UI::RenderUI()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::ResizeUI(int width, int height)
{
	WindowSize = ImVec2(width, height);

    int NewWidth = width * SceneViewScale;
    int NewHeight = height * SceneViewScale;
    SceneViewSize = ImVec2(NewWidth, NewHeight);
}

void UI::ToggleFullscreen()
{
	if (!bFullScreen)
	{
		ResizeUI(WindowSize.x, WindowSize.y);
		SceneViewScaleCache = SceneViewScale;
		SceneViewSize = WindowSize;
		SceneViewScale = 1.0;
		ResizeCallbackFunc(SceneViewSize.x, SceneViewSize.y);
		bFullScreen = true;
	}
	else
	{
		SceneViewScale = SceneViewScaleCache;
		ResizeUI(WindowSize.x, WindowSize.y);
		ResizeCallbackFunc(SceneViewSize.x, SceneViewSize.y);
		bFullScreen = false;
	}

	//Tell Application to change fbos' sizes
	

}
