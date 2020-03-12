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
	bShowSettings = false;
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

    //Main
    ImGuiWindowFlags main_window_flags = 0;
    main_window_flags |= ImGuiWindowFlags_MenuBar;
    main_window_flags |= ImGuiWindowFlags_NoResize;
    main_window_flags |= ImGuiWindowFlags_NoScrollbar;
    main_window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowSize(WindowSize);
    ImGui::SetNextWindowPosCenter();

    ImGui::Begin("Main", nullptr, main_window_flags);

	// Main Window menu
    DrawMainBar();
	if (bShowSettings)
	{
		DrawSettings();
	}
	// Main Window menu 

    DrawSceneTree();
    ImGui::SameLine();
    DrawSceneEditor();
    ImGui::SameLine();
    DrawInspector();

    //Log and stuff
    ImGui::BeginChild("LowerTabs", ImVec2(WindowSize.x, WindowSize.y /4), true);

	//TABS**************************************************
    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    ImGui::BeginTabBar("MyTabBar", tab_bar_flags);
        if (ImGui::BeginTabItem("Content Viewer"))
        {
            ImGui::Text("This is the Avocado tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Console"))
        {
            ImGui::Text("This is the Broccoli tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Whatever"))
        {
            ImGui::Text("This is the Cucumber tab!\nblah blah blah blah blah");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();


    ImGui::EndChild();

    ImGui::End(); //main
    //int VerticalSizeAdj = 45;

    ImGui::ShowDemoWindow();

}

void UI::DrawSceneEditor()
{
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    window_flags |= ImGuiWindowFlags_NoScrollbar;
    window_flags |= ImGuiWindowFlags_ChildWindow;

    ImVec2 VecScreen = SceneViewSize;
	int VerticalSizeAdj = 25;
	if (bFullScreen)
	{
		ImGui::SetNextWindowPosCenter();
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		VerticalSizeAdj = 17;
	}
		
	VecScreen.y += VerticalSizeAdj;

	ImGui::SetNextWindowSize(VecScreen);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 1.0f);

    ImGui::BeginChild("id", VecScreen, true, window_flags);

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
    ImGui::EndChild();


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

void UI::ToggleSettingsWindow()
{
	bShowSettings = !bShowSettings;
}

void UI::DrawSceneTree()
{
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::BeginChild("Scene Tree", ImVec2(WindowSize.x / 6, WindowSize.y / 1.5), true/*, window_flags*/);
    //auto enties = ecs.GetEntities();

    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow
                                           | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    static int selection_mask = (1 << 2);
    int node_clicked = -1;
    int i = 0;
    for (int j = 0; j < 4; j ++)
    {
        // Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
        ImGuiTreeNodeFlags node_flags = base_flags;
        const bool is_selected = (selection_mask & (1 << i)) != 0;
        if (is_selected)
            node_flags |= ImGuiTreeNodeFlags_Selected;

        //RenderComponent* RC = ecs.GetComponent<RenderComponent>(entity);
        //if (RC)
        //{
            // Items 0..2 are Tree Node
            bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, "foo", i);
            if (ImGui::IsItemClicked())
            {
            //    showInspector = true;
                node_clicked = i;
             //   selectedEntity = entity;
            }

            if (node_open)
            {
                ImGui::BulletText("Child1");
                ImGui::BulletText("Child2");
                ImGui::TreePop();
            }
        //}
        i++;
    }
    if (node_clicked != -1)
    {
        // Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
        if (ImGui::GetIO().KeyCtrl)
            selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
        else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
            selection_mask = (1 << node_clicked);           // Click to single-select
    }

    ImGui::EndChild();
    ImGui::PopStyleVar();
}

void UI::DrawMainBar()
{

    // Menu Bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("Save");
            ImGui::MenuItem("Load");
			if (ImGui::MenuItem("Settings"))
			{
				ToggleSettingsWindow();
			}
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
}

void UI::DrawInspector()
{
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
    ImGui::BeginChild("SceneTree", ImVec2(WindowSize.x / 6.5, WindowSize.y / 1.5), true/*, window_flags*/);
    ImGui::EndChild();
    ImGui::PopStyleVar();
}

void UI::HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

void UI::DrawSettings()
{
	//ImGui::SetNextWindowSize(ImVec2(WindowSize.x/6, WindowSize.y/6));
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
	
	ImGui::Begin("Settings", 0, window_flags);
	ImGui::SliderInt("BlurCycles", &this->PostProcessingSettings.BlurCycles, 1, 50);
	ImGui::SameLine();
	HelpMarker("CTRL+click to input value.");
	ImGui::SliderFloat("PixelVelocityMult", &this->PostProcessingSettings.PixelVelocityMult, 0.01, 1.0);
	ImGui::SameLine();
	HelpMarker("CTRL+click to input value.");
	ImGui::End();
}
