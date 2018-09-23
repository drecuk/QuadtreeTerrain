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
//	leaving the world, seeking and going after snacks
//
//	##########################################################

#ifndef PREY_H
#define PREY_H

#include "Grid.h"
#include "OGLUtil.h"
#include "Agent.h"


/****************************** PROTOTYPES ******************************/
class Prey: public Agent
{
private:

protected:

  // targeted prey (snack)
  int _preyID;
  int _distanceToTarget;
  float fov;

public:
  // ------------------- constructors destructors
  Prey();
  Prey(int _id, float origX, float origY, float origZ, float speed);
  ~Prey();

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
