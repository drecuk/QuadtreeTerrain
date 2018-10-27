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

#include "OGLUtil.h"
#include "Emitter.h"

Emitter::Emitter(): Object(-1)
{
	fScale = 10.0f;

	vPos.x = 0.0f;
	vPos.y = 0.0f;
	vPos.z = 0.0f;
}

Emitter::Emitter(int _id, float origX, float origY, float origZ): Object(_id)
{
	fScale = 10.0f;

	vPos.x = origX;
	vPos.y = origY;
	vPos.z = origZ;
}

Emitter::~Emitter()
{
	_grid = NULL;
	_emitters = NULL;
	_terrain = NULL;
  cout<<"Emitter destroyed!"<<endl;
}

void Emitter::render()
{
	// ** glIdentity must be called here so that
	// the matrix transform is reset to identity matrix made to this Emitter
	// so that it doesn't affect all other Emitters later
	// try removing this and see
	//glLoadIdentity();

	glPushMatrix();
	  // Reset the Emitter matrix (loading identity)
		matPos.identity();

		// set translation and rotation matrix
		matPos.translate(vPos.x, vPos.y, vPos.z);

		glMultMatrixf(matPos.matrix);
			DrawObject(1.0f, 0.0f, 0.0f);
	glPopMatrix();
}

void Emitter::update()
{
	placeAgentOnTerrain();
}


void Emitter::getGrid(Grid *grid)
{
	_grid = grid;
}

void Emitter::getEmitters(Emitter **emitters, int size)
{
  _noOfEmitters = size;
  _emitters = emitters;
}

void Emitter::placeAgentOnTerrain()
{
	// skate on top of terrain
	float dist = _terrain->distanceToPlane(vPos);
	if (dist <= 0.0)	// the box is under the plane
		vPos.y = vPos.y + fabs(dist);
	else
		vPos.y = vPos.y - fabs(dist);
}

void Emitter::getTerrain(QTTerrain *terrain)
{
	_terrain = terrain;
}

void Emitter::DrawObject(float red, float green, float blue)
{
	glColor3f(red, green, blue);		// set colour to red

	//glVertex3f(0.0f, 0.0f, -0.5f);
	//glVertex3f(-0.5f, 0.0f, 0.5f);
	//glVertex3f(0.5f, 0.0f, 0.5f);

	glLineWidth(3.0f);
	glBegin(GL_TRIANGLES);				// draw the triangle
		// right
		glVertex3f(0.0f, 0.0f, -1.0f*fScale);
		glVertex3f(0.0f, 0.5f*fScale, 0.0f);
		glVertex3f(0.5f*fScale, 0.0f, 0.0f);
		// left
		glVertex3f(0.0f, 0.0f, -1.0f*fScale);
		glVertex3f(-0.5f*fScale, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.5f*fScale, 0.0f);
		// back
		glVertex3f(0.5f*fScale, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.5f*fScale, 0.0f);
		glVertex3f(-0.5f*fScale, 0.0f, 0.0f);
		// under
		glVertex3f(0.0f, 0.0f, -1.0f*fScale);
		glVertex3f(0.5f*fScale, 0.0f, 0.0f);
		glVertex3f(-0.5f*fScale, 0.0f, 0.0f);
	glEnd();
	glLineWidth(0.2f);
}
