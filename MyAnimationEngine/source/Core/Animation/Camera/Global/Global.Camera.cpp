#include "Global.Camera.h"
#include <glm/gtc/matrix_transform.hpp>


Camera_Global::Camera_Global() {
	Pos.SetAll({ 0.f,1.f,-2.4f });
	Front.SetAll({ 0.f,0.f,-1.f });
	Up.SetAll({ 0.f,1.f,0.f });

	yaw.SetAll(-90.f);
	pitch.SetAll(0.f);
	fov.SetAll(45.f);

	roll.SetAll(0.f);
	delta_yaw = 0.f;
}

Camera_Global::~Camera_Global() {

}


glm::mat4 Camera_Global::getMatrixView() {
	return glm::lookAt(
		Pos.GetLerped(),
		Pos.GetLerped() + Front.GetLerped(),
		Up.GetLerped()
	);
}


float Camera_Global::getFOV() {
	return fov.GetLerped();
}
glm::vec3 Camera_Global::getPos() {
	return Pos.GetLerped();
}
glm::vec3 Camera_Global::getFront() {
	return Front.GetLerped();
}
glm::vec3 Camera_Global::getUp() {
	return Up.GetLerped();
}
glm::vec3 Camera_Global::getRight() {
	return glm::normalize(glm::cross(Front.GetLerped(), Up.GetLerped()));
}


void Camera_Global::UpdateMovement(float dt) {
	float cameraSpeed = 0.8f * dt;


	bool isSpeeding			= ImGui::IsKeyDown(ImGuiKey_LeftShift);
	bool isMoving_Forward	= ImGui::IsKeyDown(ImGuiKey_W);
	bool isMoving_Backward	= ImGui::IsKeyDown(ImGuiKey_S);
	bool isMoving_Left		= ImGui::IsKeyDown(ImGuiKey_A);
	bool isMoving_Right		= ImGui::IsKeyDown(ImGuiKey_D);
	bool isMoving_Up		= ImGui::IsKeyDown(ImGuiKey_Space);
	bool isMoving_Down		= ImGui::IsKeyDown(ImGuiKey_LeftCtrl);

	if (isSpeeding)
		cameraSpeed *= 25.f;

	glm::vec3 currentPos = Pos.GetReal();
	glm::vec3 frontReal = Front.GetReal();
	glm::vec3 upReal = Up.GetReal();

	if (isMoving_Forward) currentPos += cameraSpeed * frontReal;
	if (isMoving_Backward) currentPos -= cameraSpeed * frontReal;
	if (isMoving_Left) currentPos -= cameraSpeed * glm::normalize(glm::cross(frontReal, upReal));
	if (isMoving_Right) currentPos += cameraSpeed * glm::normalize(glm::cross(frontReal, upReal));
	if (isMoving_Up) currentPos += cameraSpeed * glm::normalize(upReal);
	if (isMoving_Down) currentPos -= cameraSpeed * glm::normalize(upReal);

	Pos.Set(currentPos);
}
void Camera_Global::UpdateLook(glm::vec2 delta) {

	float sensitivity = -0.02f;
	float xoffset = static_cast<float>(delta.x) * sensitivity;
	float yoffset = static_cast<float>(delta.y) * sensitivity * -1.f;

	float newYaw = yaw.GetReal() + xoffset;
	float newPitch = pitch.GetReal() + yoffset;

	if (newPitch > 89.0f)  newPitch = 89.0f;
	if (newPitch < -89.0f) newPitch = -89.0f;

	yaw.Set(newYaw);
	pitch.Set(newPitch);
	delta_yaw = xoffset;

	float r_yaw = glm::radians(newYaw);
	float r_pitch = glm::radians(newPitch);

	glm::vec3 front;
	front.x = std::cos(r_yaw) * std::cos(r_pitch);
	front.y = std::sin(r_pitch);
	front.z = std::sin(r_yaw) * std::cos(r_pitch);

	Front.Set(glm::normalize(front));

}
void Camera_Global::UpdateFOV(float delta_wheel) {

	float wheelValue = 2.f * delta_wheel;
	float currentFov = fov.GetReal();

	if (currentFov >= 1.0f && currentFov <= 90.0f)
		currentFov -= wheelValue;
	if (currentFov <= 1.0f)
		currentFov = 1.0f;
	if (currentFov >= 90.0f)
		currentFov = 90.0f;

	fov.Set(currentFov);
}


void Camera_Global::UpdateRoll(float dt) {
	float strafe_dir = 0.f;

	bool isMoving_Left = ImGui::IsKeyDown(ImGuiKey_A);
	bool isMoving_Right = ImGui::IsKeyDown(ImGuiKey_D);

	if (isMoving_Left) strafe_dir -= 1.f;
	if (isMoving_Right) strafe_dir += 1.f;

	float strafe_roll_max = 5.f;
	float strafe_roll = -strafe_dir * strafe_roll_max;

	if (dt == 0.f) dt = 0.016f;
	float yaw_rate = delta_yaw / dt;

	float turn_roll_factor = 0.025f;
	float turn_roll = yaw_rate * turn_roll_factor;

	float target_roll = strafe_roll + turn_roll;
	roll.Set(target_roll);
}


void Camera_Global::UpdateUp() {
	glm::vec3 world_up(0.f, 1.f, 0.f);
	glm::vec3 front = Front.GetLerped();
	glm::vec3 right = glm::normalize(glm::cross(front, world_up));
	glm::vec3 up_level = glm::normalize(glm::cross(right, front));

	float phi = glm::radians(roll.GetLerped());
	glm::vec3 new_up = up_level * std::cos(phi) + right * std::sin(phi);

	Up.Set(glm::normalize(new_up));
}



void Camera_Global::UpdateLerp(float dt) {

	float speed = 16.f;
	float time = 1.f - std::exp(-speed * dt);
	glm::vec3 timeVec(time);

	Pos.Update(timeVec);
	Front.Update(timeVec);
	Up.Update(timeVec);
	yaw.Update(time);
	pitch.Update(time);
	fov.Update(time);
	roll.Update(time);

}


void Camera_Global::Update(float dt, glm::vec2 delta_mouse, float delta_wheel) {

	UpdateMovement(dt);
	UpdateLook(delta_mouse);
	UpdateFOV(delta_wheel);
	UpdateRoll(dt);
	UpdateLerp(dt);
	UpdateUp();

}