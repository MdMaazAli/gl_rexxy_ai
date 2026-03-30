#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class TelemetryDashboard {
public:
    // Lifecycle
    void Init(GLFWwindow* window);
    void Render(int currentFrames, float currentSurvivalTime, 
                int highScoreFrames, float maxSurvivalTime, 
                const glm::vec3& cameraPos);
    void Shutdown();
};