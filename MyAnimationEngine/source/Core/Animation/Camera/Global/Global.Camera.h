#ifndef GLOBAL_CAMERA_H
#define GLOBAL_CAMERA_H

#include <glm/glm.hpp>
#include "Utils/LerpedValue/LerpedValue.h"
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_internal.h"

class Camera_Global {
public:
	Camera_Global();
	~Camera_Global();

	void Update(float dt, glm::vec2 delta_mouse, float delta_wheel);

	glm::mat4 getMatrixView();
	float	  getFOV();
	glm::vec3 getPos();
	glm::vec3 getFront();
	glm::vec3 getUp();
	glm::vec3 getRight();

private:
	LerpedValue<float> yaw, pitch, roll;
	LerpedValue<float> fov;
	LerpedValue<glm::vec3> Pos, Front, Up;

	float delta_yaw;

	float lastX, lastY;
	bool firstMouse = true;

	void UpdateMovement(float dt);
	void UpdateLook(glm::vec2 delta_mouse);
	void UpdateUp();
	void UpdateRoll(float dt);
	void UpdateFOV(float delta_wheel);
	void UpdateLerp(float dt);
};


#endif