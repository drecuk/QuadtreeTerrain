//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ OpenGL Camera Class
//
//  This is a simple camera class using Euler angles and a
//  limited set of parameters for controlling basic movements
//	gimbal lock is not resolved but angle-limit is implemented
//
//	##########################################################

#include "Camera.h"

using namespace std;

Camera::Camera()
{
	cout<<"---------------------------------->> Creating Camera"<<endl;
	pitch = roll = 0.0f;
	yaw = -90.0f;	// facing the negative z axis

	// set initial position at origin
	x = vPos.x = 0.0f;
	y = vPos.y = 30.0f;
	z = vPos.z = 60.0f;

	// initial LookAt target
	tx = 0.0f;
	ty = 0.0f;
	tz = -1.0f;	// looking into the z negative axis

	// prevPos is for recording the world position
	prevPos = Vector3f(vPos.x,vPos.y,vPos.z);

	// set eye height
	eyeHeight = 0.0f;

	// speed
	speed = 0.5f;
	maxSpeed = 1.5f;

	// angles
	rotAngle = 0.2f;
	pitchAngle = 1.0f;
}


Camera::Camera(Vector3f initialPos, Vector3f initialTarget, float _speed, float _rotAngle, float _eyeHeight)
{
	cout<<"---------------------------------->> Creating Camera"<<endl;
	pitch = roll = 0.0f;
	yaw = -90.0f;	// facing the negative z axis

	vPos = initialPos;		// assign the position parameter

	// for recording the world position
	// this creates a relative reference point to move for the initial pos
	prevPos = initialPos;

	// initial position
	x = initialPos.x;
	y = initialPos.y;
	z = initialPos.z;

	// initial target
	tx = initialTarget.x;
	ty = initialTarget.y;
	tz = initialTarget.z;

	eyeHeight = _eyeHeight;

	// speed
	speed = _speed;		// assign the speed
	maxSpeed = 2.0f;		// assign the max speed

	rotAngle = _rotAngle;	// assign the angle rotate speed
	pitchAngle = 1.0f;		// assign pitch angle
}

Camera::~Camera()
{
	cout<<"Camera destroyed"<<endl;
}

Vector3f Camera::getPosition()
{
	return Vector3f(x, y, z);
}

void Camera::print()
{
	cout<<">> position: "<<x<<" "<<y<<" "<<z<<" | target: "<<tx<<" "<<ty<<" "<<tz<<endl;
}

void Camera::update()
{
	// limit yaw and pitch
	if ( (yaw > 360.0f) || (yaw < -360.0f) ) {yaw = 0.0f;}
	if (pitch >90.0f) pitch = 90.0f;
	if (pitch <-90.0f) pitch = -90.0f;

	// calculate yaw and pitch from angle
	cosYaw = cos(DEG2RAD(yaw));
	sinYaw = sin(DEG2RAD(yaw));
	sinPitch = sin(DEG2RAD(pitch));
	cosPitch = cos(DEG2RAD(pitch));

	// record previous position
	prevPos.x = x;
	prevPos.z = z;

	// calculate next position
	x = prevPos.x + speed*cosYaw;
	y = eyeHeight;					// this is calculated in Main.cpp (y = vPos.y);
	y = vPos.y + eyeHeight;
	z = prevPos.z + speed*sinYaw;

	// limit speed
	speed = speed * 0.9f;
	if (speed < 0.0f) speed = 0.0f;

	if (speed > 0.9f) speed = 0.9f;

	// Calculate lookAt vector
	tx = x + cosYaw;
	ty = y + sinPitch;
	tz = z + sinYaw;

	//cout<<"Camera Origin:"<<x<< " "<<y<<" "<<" "<<z<<" Target:"<<tx<<" "<<ty<<" "<<tz<<endl;
}

float Camera::DEG2RAD(float angle)
{
	return angle * (PI/180.0f);
}

/*
Note that camera movement is reversed.
We are trying to move the whole scene while the camera remains static.
*/

void Camera::setMaxSpeed(float value)
{
	maxSpeed = value;
}

void Camera::setEyeHeight(float value)
{
	eyeHeight = value;
}

void Camera::moveForward()
{
	speed += 3.0f;
	//cout<<"camera move forward"<<vPos.z<<endl;

	//worldMatrix.print();
}

void Camera::moveBackward()
{
	speed -= 3.0f;
	//cout<<"camera move backward"<<vPos.z<<endl;
}

void Camera::ascend()
{
	vPos.y += 5.0f;
	//cout<<"camera move forward"<<vPos.z<<endl;
	//worldMatrix.print();
}

void Camera::descend()
{
	vPos.y -= 5.0f;
	//cout<<"camera move backward"<<vPos.z<<endl;
}

void Camera::rotateLeft()
{
	yaw -= rotAngle;
}

void Camera::rotateRight()
{
	yaw += rotAngle;
}

void Camera::pitchUp()
{
	pitch += pitchAngle;
}

void Camera::pitchDown()
{
	pitch -= pitchAngle;
}

void Camera::mousePitch(int value)
{
	pitch -= value;
}
void Camera::mouseYaw(int value)
{
	yaw += value;
}
