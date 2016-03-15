#ifndef QUATCAM_HPP
#define QUATCAM_HPP
#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <atlas/utils/Camera.hpp>
#include <atlas/utils/Geometry.hpp>
enum CameraDirection {
	UP, DOWN, LEFT, RIGHT, FORWARD, BACK, LOOKLEFT, LOOKRIGHT, ROLLCCW, ROLLCW, TILTUP, TILTDOWN
};
class QuatCamera : public atlas::utils::Camera {
public:
	QuatCamera();
	QuatCamera(glm::vec3 pos, glm::vec3 up_, glm::vec3 forward_);
	~QuatCamera();
	
	void input(CameraDirection dir_);
	glm::mat4 getCameraMatrix();
	void setPosition(glm::vec3 p);
	void setLookat(glm::vec3 loc);
	void mouseUpdate(glm::vec2 newMousePosition);
	void updateSlerp(atlas::utils::Time const& t);
	void startSlerp(GLfloat duration);
	bool isSlerping();
	void setTarget(glm::vec3 loc);
	glm::vec3 getPosition();
private:
	//Private Function
	glm::quat vectorMulQuat(glm::quat q, glm::vec3 r);
	void doSlerp(atlas::utils::Time const& t);
	void LookAt(glm::vec3 lookVector);
	glm::vec3 getLeft();
	glm::vec3 getRight();
	bool equals(glm::vec3 a, glm::vec3 b);
	void rotateX(GLfloat angle); //Pitch
	void rotateY(GLfloat angle); //Yaw
	glm::vec3 rotate(glm::vec3 source, GLfloat angle, glm::vec3 axis);
	glm::vec3 rotate(glm::vec3 source, glm::quat q);
	void move(glm::vec3 dir, GLfloat amt);
	glm::vec3 getNormalized(glm::vec3 V);

	//Members
	glm::vec2 oldMousePosition;
	glm::vec3 position, forward, up, yAxis, target;
	glm::quat rotation, start, current;
	GLfloat MOVEMENT_SPEED, ROTATION_SPEED, SPIN_SPEED, SLERP_DURATION, slerpProgress;
	bool slerping;
};
#endif