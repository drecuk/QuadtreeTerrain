//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class Integrating OpenGL
//
//  This example shows how to integrate OpenGL with Objects
//  encapsulating all internal functions, essentially creating
//  a pattern of an agent in Agent-based modelling systems
//
//	Autonomy is in the free roaming, and avoidance of boundaries
//	leaving the world
//
//  Terrain function is added to this version
//
//  The Agent class prototype derives from Object
//  Prototype details are decoupled in Agent.h
//  Decoupling codes make it very easy to manage programmatically
//	##########################################################

#ifndef AGENT_H
#define AGENT_H

#include "Grid.h"
#include "OGLUtil.h"
#include "Object.h"
#include "Category.h" // for managing agent types during simulation
#include "QTTerrain.h"

/****************************** PROTOTYPES ******************************/
class Agent: public Object
{
protected:
  // movement variables
  float fAngle, fCurrAngle, fAngleSpeed, fMaxAngle;
	float fSpeed, fMaxSpeed;
	float fMovement, fFriction;
  float fScale; // scale of the graphical representation

  // Matrix and Vector transforms
	Matrix4x4 matPos;  // position Matrix
	Matrix4x4 matRot;  // rotation matrix

  // movement flags
	bool isForward, isBackward, isRight, isLeft, isMoving;

  // internal reference to global grid
  Grid *_grid;

  // allows access of pointer to agents in main.cpp
  Agent **_agents;
  int _noOfAgents;

  // World Boundaries
  float _top; float _bottom; float _left; float _right;

  // access to terrain using pointer
  QTTerrain *_terrain;

public:
  // ------------------- constructors destructors
  Agent();
  Agent(int _id, float origX, float origY, float origZ, float speed);
  ~Agent();

  // ------------------- update functions
  virtual void render();
  virtual void update();
  SpeciesType speciesType;

  // ------------------- agent functions
  virtual void autonomy();
  void getGrid(Grid *grid);
  // get access to all agents in the world
  void getAgents(Agent **agents, int size);

  void getTerrain(QTTerrain *terrain);

  // to be implemented in derived classes
  virtual void seek() {};
  virtual void chase() {};
  virtual void isEaten() {}; // ** new member in this Agent implementation

  // ------------------- movement functions
  void rotateLeft(float fAngleSpeed);
  void rotateRight(float fAngleSpeed);
  void moveForward(float speed);
  void moveBackward(float speed);
  void stopForward();
  void stopBackward();
  void notMoving();

  void placeAgentOnTerrain();

  // ------------------- visual representation function
  void DrawObject(float red, float green, float blue);
};

#endif
