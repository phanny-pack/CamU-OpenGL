#pragma once
#include <glm/glm.hpp>

class Camera {
public:
    float yaw = -90.0f, pitch = -20.0f, distance = 5.0f;
    float lastX = 400, lastY = 300;
    bool dragging = false;

    glm::mat4 getViewProj(float aspect);
    void handleDrag(float dx, float dy);
};