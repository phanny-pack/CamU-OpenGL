#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Camera::getViewProj(float aspect) {
    glm::vec3 target = glm::vec3(0.0f);
    glm::vec3 eye;
    eye.x = distance * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    eye.y = distance * sin(glm::radians(pitch));
    eye.z = distance * cos(glm::radians(pitch)) * sin(glm::radians(yaw));

    glm::mat4 view = glm::lookAt(eye, target, glm::vec3(0, 1, 0));
    glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
    return proj * view;
}

void Camera::handleDrag(float dx, float dy) {
    yaw += dx * 0.1f;
    pitch -= dy * 0.1f;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
}