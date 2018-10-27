//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class Integrating OpenGL
//
//  The Emitter class is used for defining local environmental variables
//	E.g., local climate, temperature, pollutants, etc.
//
//  Prototype details are decoupled in Emitter.h
//
//	##########################################################

#ifndef EMITTER_H
#define EMITTER_H

#include "Grid.h"
#include "OGLUtil.h"
#include "Object.h"
#include "QTTerrain.h"
#include "Agent.h"

/****************************** PROTOTYPES ******************************/
class Emitter: public Object
{
protected:
	float fScale; // scale of the graphical representation

  // Matrix and Vector transforms
	Matrix4x4 matPos;  // position Matrix
	Matrix4x4 matRot;  // rotation matrix61`123

  // internal reference to global grid
  Grid *_grid;

  // allows access of pointer to agents in main.cpp
  Emitter **_emitters;
  int _noOfEmitters;

  // World Boundaries
  float _top; float _bottom; float _left; float _right;

  // access to terrain using pointer
  QTTerrain *_terrain;

public:
  // ------------------- constructors destructors
  Emitter();
  Emitter(int _id, float origX, float origY, float origZ);
  ~Emitter();

  // ------------------- update functions
  virtual void render();
  virtual void update();

  // access to objects in the world
  void getGrid(Grid *grid);
  void getEmitters(Emitter **emitters, int size);
  void getTerrain(QTTerrain *terrain);

  // ------------------- movement functions
  void placeAgentOnTerrain();

  // ------------------- visual representation function
  void DrawObject(float red, float green, float blue);
};

#endif
