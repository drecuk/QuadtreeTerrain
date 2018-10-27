//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class Integrating OpenGL
//
//  Snacks are eaten by Prey
//	Autonomy is in the auto-spawn in random locations
//
//	##########################################################

#ifndef SNACK_H
#define SNACK_H

#include "Grid.h"
#include "OGLUtil.h"
#include "Agent.h"

/****************************** PROTOTYPES ******************************/
class Snack: public Agent
{
protected:
  bool _isEaten;

public:
  // ------------------- constructors destructors
  Snack();
  Snack(int _id, float origX, float origY, float origZ, float speed);
  ~Snack();

  // ------------------- update functions
  void render();
  void update();

  // ------------------- utility functions
  void seek() {}
  void chase() {}
  void autonomy();
  void isEaten();


  // ------------------- visual representation function
  void DrawObject(float red, float green, float blue);
};

#endif
