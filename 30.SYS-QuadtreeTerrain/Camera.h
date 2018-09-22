//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ OpenGL Camera Class header file
//
//  This is a simple camera class using Euler angles and a
//  limited set of parameters for controlling basic movements
//	gimbal lock is not resolved but angle-limit is implemented
//
//	##########################################################

/****************************** INCLUDES ******************************/
#ifndef CAMERA_H
#define CAMERA_H

#include "OGLUtil.h"

/****************************** PROTOTYPES ******************************/
class Camera
{
private:
	// Matrix4x4 matPos;
	// Matrix4x4 matRot;
	float 	eyeHeight;
	float 	speed, maxSpeed;
	float 	rotAngle, pitchAngle;
	bool 		isForward, isBackward, isRight, isLeft, isMoving;

public:
	// Matrix4x4 worldMatrix;
	float 	yaw, pitch, roll;	// rotation
	Vector3f vPos;
	Vector3f prevPos;			// previous position, prevents the origin phenomena
	Vector3f lookAt;

	float tx, ty, tz;			// camera lookAt target
	float x, y, z;				// camera position
	float cosYaw, sinYaw, sinPitch, cosPitch;

	Camera();
	Camera(Vector3f initialPos, Vector3f initialTarget, float _speed, float _rotAngle, float _eyeHeight);
	~Camera();

	Vector3f getPosition();
	void print();
	float DEG2RAD(float angle);
	void update();
	void setEyeHeight(float value);
	void setMaxSpeed(float value);
	void moveForward();
	void moveBackward();
	void ascend();
	void descend();
	void rotateLeft();
	void rotateRight();
	void pitchUp();
	void pitchDown();
	void mousePitch(int value);
	void mouseYaw(int value);

};

#endif
