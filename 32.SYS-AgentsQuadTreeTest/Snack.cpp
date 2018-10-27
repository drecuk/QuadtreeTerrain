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

#include "OGLUtil.h"
#include "Snack.h"

Snack::Snack(int _id, float origX, float origY, float origZ, float speed): Agent(_id, origX, origY, origZ, speed)
{
	fScale = 10.0f;
	objectType = OBJTYPE_AGENT;
	
	vPos.x = origX;
	vPos.y = origY;
	vPos.z = origZ;
}

Snack::~Snack()
{
  cout<<"Snack destroyed!"<<endl;
}


void Snack::render()
{
	// ** glIdentity must be called here so that
	// the matrix transform is reset to identity matrix made to this Snack
	// so that it doesn't affect all other Snacks later
	// try removing this and see
	//glLoadIdentity();

	glPushMatrix();
	  // Reset the agent matrix (loading identity)
		matPos.identity();

		// set translation and rotation matrix
		matPos.translate(vPos.x, vPos.y, vPos.z);
		fCurrAngle += 0.3f;
		matRot.rotateY(fCurrAngle);

		// load position matrix and add rotation
		//glLoadMatrixf(matPos.matrix);
		glMultMatrixf(matPos.matrix);
		glMultMatrixf(matRot.matrix);
			DrawObject(0.0f, 1.0f, 0.0f);
	glPopMatrix();
}

void Snack::update()
{
	autonomy();
}

void Snack::autonomy()
{
	if(_isEaten)
	{
		_isEaten = false;
		int min = _grid->getBottom();
		int max = _grid->getBottom() + _grid->getBottom();

		int newX = (rand()%max)-min;
		int newZ = (rand()%max)-min;

		//cout<<"newx,newy"<<newX<<","<<newY<<endl;

		vPos.x = newX;
		vPos.z = newZ;
	}

	placeAgentOnTerrain();
}
void Snack::isEaten()
{
	_isEaten = true;
}

// void Snack::seek()
// {
//
// }
//
// void Snack::chase()
// {
//
// }

void Snack::DrawObject(float red, float green, float blue)
{
	glColor3f(red, green, blue);		// set colour to red

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// head pointing towards the positive x axis at the start
	glLineWidth(2.0f);
	glBegin(GL_TRIANGLES);				// draw the triangle
		// right
		glVertex3f(0.0f, 2.0f*fScale, 0.0f);
		glVertex3f(0.0f, 0.0f, 1.0f*fScale);
		glVertex3f(-1.0f*fScale, 0.0f, 0.0f);
		// left
		glVertex3f(0.0f, 2.0f*fScale, 0.0f);
		glVertex3f(1.0f*fScale, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 1.0f*fScale);
		// back
		glVertex3f(0.0f, 2.0f*fScale, 0.0f);
		glVertex3f(1.0f*fScale, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, -1.0f*fScale);
		// belly
		glVertex3f(0.0f, 2.0f*fScale, 0.0f);
		glVertex3f(0.0f, 0.0f, -1.0f*fScale);
		glVertex3f(-1.0f*fScale, 0.0f, 0.0f);
	glEnd();
	glLineWidth(0.2f);

}
