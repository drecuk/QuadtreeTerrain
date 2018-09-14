//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Grid Class for OpenGL
//
//	The grid is used to add visual reference point for Agent worlds
//	The grid also sets the boundary for the simulation world
//
//	##########################################################

#ifndef GRID_H
#define GRID_H

/******************************** includes ********************************/
#include "OGLUtil.h"

/******************************** Class ********************************/
class Grid
{
private:
	float gridWidth;	// the width boundary of grid grid
	float gridHeight;	// the height boundary of the grid
	float noSegments;	// the number of segments
	float gridSpace;	// gridWidth/noSegments
	bool showAxis;		// flag for axis

	// boundaries
	float _top;
	float _bottom;
	float _left;
	float _right;

public:
	Grid();								// the empty default constructor
	Grid(float width, float height, float segments);
	void setAxis(bool state);

	// void setBoundary(float top, float bottom, float left, float right);
	void calculateBoundaries();
	bool isInBoundary(Vector3f pos, float buffer);
	float getTop();
	float getBottom();
	float getLeft();
	float getRight();

	void render();				// Draw Grid


	~Grid();							// the destructor
};

#endif
