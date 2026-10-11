#include "Global.Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

Camera_Global::Camera_Global() {
    Pos.SetAll({ 0.f, 1.f, -2.4f });
    Rot.SetAll(glm::quatLookAt(glm::vec3(0.f, 0.f, 1.f), glm::vec3(0.f, 1.f, 0.f)));

    fov.SetAll(45.f);
    lean.SetAll(0.f);
    delta_yaw = 0.f;
}

Camera_Global::~Camera_Global() {}

glm::quat Camera_Global::getRenderOrientation() const {
    return Rot.GetLerped() * glm::angleAxis(glm::radians(lean.GetLerped()),
        glm::vec3(0.f, 0.f, 1.f));
}

glm::mat4 Camera_Global::getMatrixView() {
    const glm::quat orient = getRenderOrientation();
    const glm::vec3 pos = Pos.GetLerped();
    const glm::vec3 front = orient * glm::vec3(0.f, 0.f, 1.f);
    const glm::vec3 up = orient * glm::vec3(0.f, 1.f, 0.f);
    return glm::lookAt(pos, pos + front, up);
}

float Camera_Global::getFOV() { return fov.GetLerped(); }
glm::vec3 Camera_Global::getPos() { return Pos.GetLerped(); }

glm::vec3 Camera_Global::getFront() {
    return Rot.GetLerped() * glm::vec3(0.f, 0.f, 1.f);
}

glm::vec3 Camera_Global::getUp() {
    return getRenderOrientation() * glm::vec3(0.f, 1.f, 0.f);
}

glm::vec3 Camera_Global::getRight() {
    return glm::normalize(glm::cross(getFront(), getUp()));
}

glm::quat Camera_Global::getGizmoRotation() {
    return glm::normalize(Rot.GetLerped());
}

void Camera_Global::setFromGizmo(const glm::vec3& pos, const glm::quat& rot, bool instant) {
    const glm::quat q = glm::normalize(rot);
    if (instant) { Pos.SetAll(pos); Rot.SetAll(q); }
    else { Pos.Set(pos);    Rot.Set(q); }
}

void Camera_Global::UpdateMovement(float dt) {
    float cameraSpeed = 0.8f * dt;

    const bool isSpeeding = ImGui::IsKeyDown(ImGuiKey_LeftShift);
    const bool isMoving_Forward = ImGui::IsKeyDown(ImGuiKey_W);
    const bool isMoving_Backward = ImGui::IsKeyDown(ImGuiKey_S);
    const bool isMoving_Left = ImGui::IsKeyDown(ImGuiKey_A);
    const bool isMoving_Right = ImGui::IsKeyDown(ImGuiKey_D);
    const bool isMoving_Up = ImGui::IsKeyDown(ImGuiKey_Space);
    const bool isMoving_Down = ImGui::IsKeyDown(ImGuiKey_LeftCtrl);

    if (isSpeeding) cameraSpeed *= 25.f;

    const glm::quat q = Rot.GetReal();
    const glm::vec3 front = q * glm::vec3(0.f, 0.f, 1.f);
    const glm::vec3 up = q * glm::vec3(0.f, 1.f, 0.f);
    const glm::vec3 right = glm::normalize(glm::cross(front, up));

    glm::vec3 currentPos = Pos.GetReal();

    if (isMoving_Forward)  currentPos += cameraSpeed * front;
    if (isMoving_Backward) currentPos -= cameraSpeed * front;
    if (isMoving_Left)     currentPos -= cameraSpeed * right;
    if (isMoving_Right)    currentPos += cameraSpeed * right;
    if (isMoving_Up)       currentPos += cameraSpeed * up;
    if (isMoving_Down)     currentPos -= cameraSpeed * up;

    Pos.Set(currentPos);
}

void Camera_Global::UpdateLook(glm::vec2 delta) {
    const float sensitivity = -0.02f;
    const float xoffset = static_cast<float>(delta.x) * sensitivity;
    const float yoffset = static_cast<float>(delta.y) * sensitivity * -1.f;

    delta_yaw = xoffset;
    if (xoffset == 0.f && yoffset == 0.f)
        return;

    glm::quat q = Rot.GetReal();
    glm::vec3 front = q * glm::vec3(0.f, 0.f, 1.f);
    glm::vec3 up = q * glm::vec3(0.f, 1.f, 0.f);
    glm::vec3 right = glm::normalize(glm::cross(front, up));

    const float pitch_deg = glm::degrees(std::asin(glm::clamp(front.y, -1.f, 1.f)));
    float new_pitch_deg = pitch_deg + glm::degrees(yoffset);

    if (new_pitch_deg > 89.9f) new_pitch_deg = 89.9f;
    if (new_pitch_deg < -89.9f) new_pitch_deg = -89.9f;
    if (pitch_deg >= 89.9f && new_pitch_deg >= 89.9f)  new_pitch_deg = pitch_deg;
    if (pitch_deg <= -89.9f && new_pitch_deg <= -89.9f) new_pitch_deg = pitch_deg;

    const float pitch_delta = glm::radians(new_pitch_deg - pitch_deg);

    const glm::quat yawRot = glm::angleAxis(xoffset, glm::vec3(0.f, -1.f, 0.f));
    const glm::quat pitchRot = glm::angleAxis(pitch_delta, right);

    Rot.Set(glm::normalize(yawRot * pitchRot * q));
}

void Camera_Global::UpdateLean(float dt) {
    float strafe_dir = 0.f;

    if (ImGui::IsKeyDown(ImGuiKey_A)) strafe_dir -= 0.25f;
    if (ImGui::IsKeyDown(ImGuiKey_D)) strafe_dir += 0.25f;

    if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
        strafe_dir *= 4.f;

    const float strafe_roll = -strafe_dir * 5.f;

    if (dt == 0.f) dt = 0.016f;
    const float turn_roll = (delta_yaw / dt) * 0.5f;

    lean.Set(strafe_roll + turn_roll);
}

void Camera_Global::UpdateFOV(float delta_wheel) {
    float wheelValue = 2.f * delta_wheel;
    float currentFov = fov.GetReal();

    if (currentFov >= 1.0f && currentFov <= 90.0f) currentFov -= wheelValue;
    if (currentFov <= 1.0f) currentFov = 1.0f;
    if (currentFov >= 90.0f) currentFov = 90.0f;

    fov.Set(currentFov);
}

void Camera_Global::UpdateLerp(float dt) {
    const float speed = 16.f;
    const float t = 1.f - std::exp(-speed * dt);
    const glm::vec3 tVec(t);

    Pos.Update(tVec);
    Rot.Update(t);
    fov.Update(t);
    lean.Update(t);
}

void Camera_Global::Update(float dt, glm::vec2 delta_mouse, float delta_wheel) {
    UpdateMovement(dt);
    UpdateLook(delta_mouse);
    UpdateFOV(delta_wheel);
    UpdateLean(dt);
    UpdateLerp(dt);
}
