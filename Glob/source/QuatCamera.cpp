#include "QuatCamera.h"
#include <iostream>
#include <glm/gtx/transform.hpp>

QuatCamera::QuatCamera(glm::vec3 pos, glm::vec3 up_, glm::vec3 forward_) :
	yAxis{ 0.0f, 1.0f, 0.0f },
	position{ pos },
	forward{ forward_ },
	up{ up_ },
	MOVEMENT_SPEED(0.1f), ROTATION_SPEED(0.0025f), SPIN_SPEED(40.0f),
	slerping(false)
{
	up = getNormalized(up);
	forward = getNormalized(forward);
}
//Convenience Constructor
QuatCamera::QuatCamera():
	yAxis{ 0.0f, 1.0f, 0.0f },
	position{ 7.0f, 4.0f, 20.0f },
	forward{ 0.0f, 0.0f, -1.0f },
	up{ 0.0f, 1.0f, 0.0f },
	MOVEMENT_SPEED(0.1f), ROTATION_SPEED(0.0025f), SPIN_SPEED(40.0f),
	slerping(false)
{
	up = getNormalized(up);
	forward = getNormalized(forward);
}
QuatCamera::~QuatCamera() {

}
void QuatCamera::input(CameraDirection dir_) {
	GLfloat movAmt = 0.5f;
	GLfloat rotAmt = 1.5f;

	switch (dir_) {
		case FORWARD:
			move(forward, movAmt);
			break;
		case BACK:
			move(forward, -movAmt);
			break;
		case LEFT:
			move(getLeft(), -movAmt);
			break;
		case RIGHT:
			move(getRight(), -movAmt);
			break;
		case UP:
			move(up, movAmt);
			break;
		case DOWN:
			move(up, -movAmt);
			break;
		case TILTDOWN:
			rotateX(rotAmt);
			break;
		case TILTUP:
			rotateX(-rotAmt);
			break;
		case LOOKLEFT:
			rotateY(rotAmt);
			break;
		case LOOKRIGHT:
			rotateY(-rotAmt);
			break;
		default:
			break;
	}
}
void QuatCamera::mouseUpdate(glm::vec2 newMousePosition) {
	glm::vec2 mouseDelta = newMousePosition - oldMousePosition;
	if (glm::length(mouseDelta) > 50.0f)
	{
		oldMousePosition = newMousePosition;
		return;
	}
	glm::mat4 rotator = glm::rotate(-mouseDelta.x * ROTATION_SPEED, up) * glm::rotate(-mouseDelta.y * ROTATION_SPEED, getLeft());
	forward = glm::mat3(rotator) * forward;
	oldMousePosition = newMousePosition;
}
//Aim the Camera at a target
void QuatCamera::setLookat(glm::vec3 loc) {
	target = loc;
	forward = getNormalized(loc - position);
}
void QuatCamera::setTarget(glm::vec3 loc) {
	target = loc;
}
void QuatCamera::setPosition(glm::vec3 p) {
	position = p;
}
glm::mat4 QuatCamera::getCameraMatrix() {
	return glm::lookAt(position, position + forward, up);
}
void QuatCamera::move(glm::vec3 dir, GLfloat amt) {
	position += dir * amt;
}
//Get the left vector of current camera orientation
glm::vec3 QuatCamera::getLeft() {
	return getNormalized(glm::cross(forward, up));
}
//Get the right vector of current camera orientation
glm::vec3 QuatCamera::getRight() {
	return getNormalized(glm::cross(up,forward));
}
//Performs Pitch
void QuatCamera::rotateX(GLfloat angle) {
	glm::vec3 Haxis = getNormalized(glm::cross(yAxis, forward));
	forward = rotate(forward, angle, Haxis);
	forward = getNormalized(forward);
	up = getNormalized(glm::cross(forward, Haxis));
}
//Perfroms Yaw
void QuatCamera::rotateY(GLfloat angle) {
	glm::vec3 Haxis = getNormalized(glm::cross(yAxis, forward));
	forward = rotate(forward, angle, yAxis);
	forward = getNormalized(forward);
	up = getNormalized(glm::cross(forward, Haxis));
}

//Perform a rotation using quaternion math to move the source vector around
//The axis by the angle amount
glm::vec3 QuatCamera::rotate(glm::vec3 source, GLfloat angle, glm::vec3 axis) {
	GLfloat sinHalfAngle = sin(glm::radians(angle) / 2);
	GLfloat cosHalfAngle = cos(glm::radians(angle) / 2);

	glm::quat temp;
	temp.x = axis.x * sinHalfAngle;
	temp.y = axis.y * sinHalfAngle;
	temp.z = axis.z * sinHalfAngle;
	temp.w = cosHalfAngle;

	glm::quat result = vectorMulQuat(temp, source) * glm::conjugate(temp);
	return glm::vec3{ result.x, result.y, result.z };
}
glm::vec3 QuatCamera::rotate(glm::vec3 source, glm::quat q) {
	glm::quat conj = glm::conjugate(q);
	glm::quat w = vectorMulQuat(q, source) * conj;
	return glm::vec3{w.x, w.y, w.z};
}
//Multiply the quaternion by a vector
glm::quat QuatCamera::vectorMulQuat(glm::quat q, glm::vec3 r)
{
	GLfloat w_ = -q.x * r.x - q.y * r.y - q.z * r.z;
	GLfloat x_ = q.w * r.x + q.y * r.z - q.z * r.y;
	GLfloat y_ = q.w * r.y + q.z * r.x - q.x * r.z;
	GLfloat z_ = q.w * r.z + q.x * r.y - q.y * r.x;

	return glm::quat(w_, x_, y_, z_);
}
//Normalize a vector with a safeguard for division by zero
glm::vec3 QuatCamera::getNormalized(glm::vec3 V) {
	GLfloat length = glm::length(V);
	if (length < 0.0000001) return glm::vec3{ 0.0f, 0.0f, 0.0f };
	else return glm::normalize(V);
}
//Generate a quaternion that represents the rotation required to
//Rotate to look at the location specified by the lookVector
void QuatCamera::LookAt(glm::vec3 lookVector) {
	assert(lookVector != position);

	glm::vec3 direction = glm::normalize(lookVector - position);
	float dot = glm::dot(glm::vec3(0, 0, 1), direction);
	if (fabs(dot - (-1.0f)) < 0.000001f) {
		rotation = glm::angleAxis((GLfloat)glm::degrees(M_PI), glm::vec3(0, 1, 0));
		return;
	}
	else if (fabs(dot - (1.0f)) < 0.000001f) {
		rotation = glm::quat();
		return;
	}

	float angle = -glm::degrees(acosf(dot));

	glm::vec3 cross = glm::normalize(glm::cross(glm::vec3(0, 0, 1), direction));
	rotation = glm::normalize(glm::angleAxis(angle, cross));
}
//Perform the incremental slerp operation
void QuatCamera::doSlerp(atlas::utils::Time const& t) {
	GLfloat EPSILON = 0.0001f;
	GLfloat cos = glm::dot(start, rotation);
	if (cos < 0.00000000000f) {
		rotation = -rotation;
		cos = -cos;
	}
	GLfloat theta = acos(cos);
	glm::quat result = (sin(1.0f - (slerpProgress * theta)) * start) + (sin(slerpProgress * theta) * rotation);
	result *= 1.0f / (sin(theta));
	//current = glm::slerp(current, rotation, t.deltaTime / SLERP_DURATION);
	current = result;
	forward = rotate(forward, current);
	up = rotate(up, current);
}
//Increments the slerp
void QuatCamera::updateSlerp(atlas::utils::Time const& t) {
	slerpProgress += t.deltaTime / SLERP_DURATION;
	//Slerping Complete
	if ((slerpProgress >= 1.0f) || equals(getNormalized(target - position), forward)) {
		slerping = false;
		slerpProgress = 0.0000000000f;
	}
	else {
		doSlerp(t);
	}
}
//Called by user input to begin slerping so as to end up looking at the target specified
//By the setLookAt function
void QuatCamera::startSlerp(GLfloat duration) {
	slerping = true;
	SLERP_DURATION = duration;
	slerpProgress = 0.0000000000f;
	start = glm::quat(); //Create start quaternion
	LookAt(target);
}
//A basic vector equals operation to accomodate floating point error
bool QuatCamera::equals(glm::vec3 a, glm::vec3 b) {
	GLfloat ep = 0.0001f;
	if ((abs(a.x - b.x) < ep) && (abs(a.y - b.y) < ep) && (abs(a.z - b.z) < ep)) {
		return true;
	}
	else return false;
}
bool QuatCamera::isSlerping() {
	return slerping;
}