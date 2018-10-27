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

/******************************** includes ********************************/
#include "Grid.h"
using namespace std;

/******************************** constructor ********************************/
Grid::Grid()
{
	cout<<"---------------------------------->> Generating Grids"<<endl;
	gridWidth = 60;
	gridHeight = 60;
	noSegments = 40;
	gridSpace = 60/40;

	calculateBoundaries();

	showAxis = true;

	cout<<"Width: "<<gridWidth<<" height: "<<gridHeight<<" Segments: "<<noSegments<<" Grid Space: "<<gridSpace<<endl;
}

Grid::Grid(float width, float height, float segments)
{
	cout<<"---------------------------------->> Generating Grids"<<endl;

	gridWidth = width;
	gridHeight = height;
	noSegments = segments;
	gridSpace = width/segments;

	calculateBoundaries();

	showAxis = true;

	cout<<"Width: "<<width<<" height: "<<height<<" Segments: "<<segments<<" Grid Space: "<<gridSpace<<endl;
}

void Grid::setAxis(bool state)
{
	showAxis = state;
}

/******************************** methods ********************************/
void Grid::render()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glClear(GL_COLOR_BUFFER_BIT);
	// glLoadIdentity();
	// ------------------------ DRAW GRID LINES

		// DRAW BOUNDARY LINES
		glLineWidth(2.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);																// TOP
			glVertex3f(-(gridHeight/2), 0, -gridHeight/2);
			glVertex3f(gridHeight/2, 0, -gridHeight/2);
		glEnd();
		glBegin(GL_LINES);																// BOTTOM
			glVertex3f(-(gridHeight/2), 0, gridHeight/2);
			glVertex3f(gridHeight/2, 0, gridHeight/2);
		glEnd();
		glBegin(GL_LINES);																// LEFT
			glVertex3f(-(gridHeight/2), 0, -gridHeight/2);
			glVertex3f(-gridHeight/2, 0, gridHeight/2);
		glEnd();
		glBegin(GL_LINES);																// RIGHT
			glVertex3f(gridHeight/2, 0, -gridHeight/2);
			glVertex3f(gridHeight/2, 0, gridHeight/2);
		glEnd();

		glLineWidth(0.5f);
		for(int i=0; i<=noSegments; i++)
		{
			// horizontal segment lines glVertex3f(x, y, z)
			glBegin(GL_LINES);
			glColor3f(0.5f, 0.5f, 0.5f);	// red (x axis)
				glVertex3f(-(gridWidth/2), 0, (-gridWidth/2)+(i*gridSpace)); // point 1
				glVertex3f((gridWidth/2), 0, (-gridWidth/2)+(i*gridSpace));	// point 2
			glEnd();
			// horizontal segment lines
			glBegin(GL_LINES);
			glColor3f(0.5f, 0.5f, 0.5f);	// red (x axis)
				glVertex3f((-gridHeight/2)+(i*gridSpace), 0, -(gridHeight/2));
				glVertex3f((-gridHeight/2)+(i*gridSpace), 0, (gridHeight/2));
			glEnd();
		}

		// ------------------------ DRAW AXIS
		if (showAxis)
		{
			glLineWidth(3.0f);
			glColor3f(1.0f, 0.0f, 0.0f);	// red (x axis)
			glBegin(GL_LINES);
				glVertex3f(-gridWidth/2, 0, 0);
				glVertex3f(gridWidth/2, 0, 0);
			glEnd();
			glColor3f(0.0f, 1.0f, 0.0f);	// green	(y axis)
			glBegin(GL_LINES);
				glVertex3f(0, -gridHeight/2, 0);
				glVertex3f(0, gridHeight/2, 0);
			glEnd();
			glColor3f(0.0f, 0.0f, 1.0f);	// blue	(z axis)
			glBegin(GL_LINES);
				glVertex3f(0, 0, -gridHeight/2);
				glVertex3f(0, 0, gridHeight/2);
			glEnd();
		}
}

void Grid::calculateBoundaries()
{
	_top = -gridHeight/2;
	_bottom = gridHeight/2;
	_left = -gridHeight/2;
	_right = gridHeight/2;

	cout<<"top: "<<_top<<" | bottom: "<<_bottom<<" | left: "<<_left<<" | right: "<<_right<<endl;
}

bool Grid::isInBoundary(Vector3f pos, float buffer)
{
	bool inside = false;

	if ((pos.x > _left + buffer) && (pos.x < _right - buffer))
		if ((pos.z > _top + buffer) && (pos.z < _bottom - buffer))
			inside = true;
		else
			inside = false;

	return inside;
}

float Grid::getTop()
{
	return _top;
}
float Grid::getBottom()
{
	return _bottom;
}
float Grid::getLeft()
{
	return _left;
}
float Grid::getRight()
{
	return _right;
}

/******************************** destructor ********************************/
Grid::~Grid()
{
	cout << "grid destroyed" << endl;
}
