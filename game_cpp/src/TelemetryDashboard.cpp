#include "TelemetryDashboard.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void TelemetryDashboard::Init(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
}

void TelemetryDashboard::Render(int currentFrames, float currentSurvivalTime, 
                                int highScoreFrames, float maxSurvivalTime, 
                                const glm::vec3& cameraPos) {
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Build the Window
    ImGui::Begin("Matrix Telemetry");
    
    // Engine Stats
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "--- Engine ---");
    ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
    ImGui::Text("Frame Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::Spacing();

    // Current Run
    ImGui::TextColored(ImVec4(0.0f, 1.0f, 1.0f, 1.0f), "--- Current Run ---");
    ImGui::Text("Survival Time: %.2f sec", currentSurvivalTime);
    ImGui::Text("Frames Survived: %d", currentFrames);
    ImGui::Spacing();

    // High Scores
    ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "--- High Scores ---");
    ImGui::Text("Max Time: %.2f sec", maxSurvivalTime);
    ImGui::Text("Max Frames: %d", highScoreFrames);
    ImGui::Spacing();

    // Debug
    ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "--- Camera Debug ---");
    ImGui::Text("X: %.2f | Y: %.2f | Z: %.2f", cameraPos.x, cameraPos.y, cameraPos.z);
    
    ImGui::End();

    // Render to screen
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void TelemetryDashboard::Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}