//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class
//
//  The MoveableOnQTTerrain is an updated version from the Moveable class
//	Allowing the avatar to skate on the surface of a Quadtree terrain
//
//  The MoveableOnQTTerrain class prototype derives from Object
//  Implementation details are decoupled in MoveableOnQTTerrain.cpp
//  Decoupling codes make it very easy to manage programmatically
//	##########################################################
#ifndef MOVEABLEONQTTERRAIN_H
#define MOVEABLEONQTTERRAIN_H

#include "OGLUtil.h"
#include "Object.h"
#include "QTTerrain.h"

/****************************** PROTOTYPES ******************************/
class MoveableOnQTTerrain: public Object
{
private:
  // movement variables
  float fAngle, fCurrAngle, fAngleSpeed, fMaxAngle, fScale;
	float fSpeed, fMaxSpeed;
	float fMovement, fFriction;

  // Matrix and Vector transforms
	Matrix4x4 matPos;  // position Matrix
	Matrix4x4 matRot;  // rotation matrix
	Vector3f 	vPos;    // position of the object

  // movement flags
	bool isForward, isBackward, isRight, isLeft, isMoving;

  QTTerrain *_terrain;

public:
  // constructors
  MoveableOnQTTerrain();
  MoveableOnQTTerrain(int _id, float origX, float origY, float origZ, float speed);
  ~MoveableOnQTTerrain();

  // update functions
  void render();
  void update();

  // movement functions
  Vector3f getPosition();
  void rotateLeft(float fAngleSpeed);
  void rotateRight(float fAngleSpeed);
  void moveForward(float speed);
  void moveBackward(float speed);
  void stopForward();
  void stopBackward();
  void notMoving();

  void getTerrain(QTTerrain *terrain);

  // visual representation function
  void DrawObject(float red, float green, float blue);
};

#endif
