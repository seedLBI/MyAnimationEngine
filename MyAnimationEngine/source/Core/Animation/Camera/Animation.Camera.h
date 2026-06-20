#ifndef ANIMATION_CAMERA_H
#define ANIMATION_CAMERA_H

#include <glm/glm.hpp>
#include "Core/Animation/ObjectProperties/ObjectProperties.h"
#include "Core/Animation/Keyframe/KeyframeResolver.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>

#define RAD2DEG 180.f / 3.14159265358979323846f
#define DEG2RAD 3.14159265358979323846f / 180.f

class AnimationCamera {
public:
	AnimationCamera();
	~AnimationCamera();

	glm::vec3 getFront(float time);
	glm::vec3 getUp(float time);
	glm::vec3 getPos(float time);

	glm::mat4 getViewMatrix(float time);
	glm::mat4 getProjectionMatrix(float time, float aspectRatio);

	ObjectProperties properties;
};


AnimationCamera::AnimationCamera() {
	properties.set("position",	TP3Float());
	properties.set("yaw",		TPFloat());
	properties.set("pitch",		TPFloat());
	properties.set("roll",		TPFloat());
	properties.set("fov",		TPFloat());
	properties.set("near",		TPFloat());
	properties.set("fear",		TPFloat());
	properties.set("mode",		TPBool());


	auto position = properties.get<TP3Float>("position").kr.getDefaultValue();
	position = { 0.f,0.f,0.f };
	properties.get<TP3Float>("position").kr.setDefaultValue(position);

	auto yaw = properties.get<TPFloat>("yaw").kr.getDefaultValue();
	yaw = { -90.f };
	properties.get<TPFloat>("yaw").kr.setDefaultValue(yaw);


	auto pitch = properties.get<TPFloat>("pitch").kr.getDefaultValue();
	pitch = { 0.f };
	properties.get<TPFloat>("pitch").kr.setDefaultValue(pitch);


	auto roll = properties.get<TPFloat>("roll").kr.getDefaultValue();
	roll = { 0.f };
	properties.get<TPFloat>("roll").kr.setDefaultValue(roll);



	auto fov = properties.get<TPFloat>("fov").kr.getDefaultValue();
	fov = { 90.f };
	properties.get<TPFloat>("fov").kr.setDefaultValue(fov);


	auto mode = properties.get<TPBool>("mode").kr.getDefaultValue();
	mode = { 0 };
	properties.get<TPBool>("mode").kr.setDefaultValue(mode);


}
AnimationCamera::~AnimationCamera() {

}

glm::vec3 AnimationCamera::getFront(float time) {
	glm::vec3 front;

	float yaw = properties.get<TPFloat>("yaw").kr.evaluate(time)[0] * DEG2RAD;
	float pitch = properties.get<TPFloat>("pitch").kr.evaluate(time)[0] * DEG2RAD;

	front.x = cos(yaw) * cos(pitch);
	front.y = sin(pitch);
	front.z = sin(yaw) * cos(pitch);

	return front;
}

glm::vec3 AnimationCamera::getUp(float time) {
	glm::vec3 world_up{ 0.f, 1.f, 0.f };
	glm::vec3 front = getFront(time);
	glm::vec3 right = glm::normalize(glm::cross(front, world_up));
	glm::vec3 up_level = glm::normalize(glm::cross(right, front));

	float phi = properties.get<TPFloat>("roll").kr.evaluate(time)[0] * DEG2RAD;
	return glm::normalize(glm::cos(phi) * up_level + glm::sin(phi) * right);
}

glm::vec3 AnimationCamera::getPos(float time) {
	glm::vec3 pos;

	auto pos_array = properties.get<TP3Float>("position").kr.evaluate(time);

	pos.x = pos_array[0];
	pos.y = pos_array[1];
	pos.z = pos_array[2];

	return pos;
}


glm::mat4 AnimationCamera::getViewMatrix(float time) {
	glm::vec3 pos = getPos(time);
	glm::vec3 front = getFront(time);
	glm::vec3 up = getUp(time);
	return glm::lookAt(pos, pos + front, up);
}


glm::mat4 AnimationCamera::getProjectionMatrix(float time, float aspectRatio) {
	return glm::perspective(
		glm::radians(properties.get<TPFloat>("fov").kr.evaluate(time)[0]), 
		aspectRatio, 
		properties.get<TPFloat>("near").kr.evaluate(time)[0], 
		properties.get<TPFloat>("fear").kr.evaluate(time)[0]);
}

#endif