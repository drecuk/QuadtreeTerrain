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
//	leaving the world, seeking and chasing after prey
//
//	* predator will never catch the prey in this version
//	See Prey.ccp eat Snacks.cpp for the eating implementation
//
//  The Agent class prototype derives from Object
//  Prototype details are decoupled in Agent.h
//  Decoupling codes make it very easy to manage programmatically
//	##########################################################

#ifndef PREDATOR_H
#define PREDATOR_H

#include "Grid.h"
#include "OGLUtil.h"
#include "Agent.h"


/****************************** PROTOTYPES ******************************/
class Predator: public Agent
{
private:

protected:

  // targeted prey
  int _preyID;
  int _distanceToTarget;
  float fov;

public:
  // ------------------- constructors destructors
  Predator();
  Predator(int _id, float origX, float origY, float origZ, float speed);
  ~Predator();

  // ------------------- update functions
  void render();
  void update();

  // ------------------- agent functions
  void autonomy();

  void seek();  // look for agents in vicinity
  void chase(); // target prey


  // ------------------- visual representation function
  void DrawObject(float red, float green, float blue);
};

#endif
