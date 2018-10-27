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
//  The Agent class prototype derives from Object
//  Prototype details are decoupled in Agent.h
//  Decoupling codes make it very easy to manage programmatically
//
//	UPDATES
//	- Using Quadtree terrain
//	- If left the boundary of the landscape reset position to 0,0,0
//
//	##########################################################

#include "OGLUtil.h"
#include "Agent.h"

Agent::Agent(): Object(-1)
{
  // setting standard movement, rotation speed
	fAngle = 0;
	fCurrAngle = 0;
	fAngleSpeed = 0.1f;
	fMaxAngle = 5.0f;		// increased max angle now
	fSpeed = 0.001f;
	fMaxSpeed = 0.1f;
	fMovement = 0.0f;
	fFriction = 0.99f;
	fScale = 10.0f;

	isForward = isBackward = isRight = isLeft = false;

	vPos.x = 0.0f;
	vPos.y = 0.0f;
	vPos.z = 0.0f;
}

Agent::Agent(int _id, float origX, float origY, float origZ, float speed): Object(_id)
{
  // setting standard movement, rotation speed
	fAngle = 0;
	fCurrAngle = 0;
	fAngleSpeed = 0.1f;
	fMaxAngle = 5.0f;		// increased max angle now
	fSpeed = speed;
	fMaxSpeed = 0.1f;
	fMovement = 0.0f;
	fFriction = 0.99f;
	objectType = OBJTYPE_AGENT;

	fScale = 10.0f;

	isForward = isBackward = isRight = isLeft = false;

	vPos.x = origX;
	vPos.y = origY;
	vPos.z = origZ;
}

Agent::~Agent()
{
	_grid = NULL;
	_agents = NULL;
	_terrain = NULL;
  cout<<"Agent destroyed!"<<endl;
}

// void Agent::setBoundary(float top, float bottom, float left, float right)
// {
// 	_top = top;
// 	_bottom = bottom;
// 	_left = left;
// 	_right = right;
// }

void Agent::render()
{
	// ** glIdentity must be called here so that
	// the matrix transform is reset to identity matrix made to this agent
	// so that it doesn't affect all other agents later
	// try removing this and see
	//glLoadIdentity();

	glPushMatrix();
	  // Reset the agent matrix (loading identity)
		matPos.identity();

		// set translation and rotation matrix
		matPos.translate(vPos.x, vPos.y, vPos.z);
		//fCurrAngle += 0.11f;
		matRot.rotateY(fCurrAngle);

		// load position matrix and add rotation
		//glLoadMatrixf(matPos.matrix);
		glMultMatrixf(matPos.matrix);
		glMultMatrixf(matRot.matrix);
			DrawObject(1.0f, 0.0f, 0.0f);
	glPopMatrix();
}

void Agent::update()
{
	autonomy();

	if(isForward)
	{
		fMovement -= fSpeed;
		if (fMovement <= -fMaxSpeed) fMovement = -fMaxSpeed;
		if (fMovement >= -0.0001) { fMovement = 0.0f; isForward = false; }
	}

	if(isBackward)
	{
		fMovement += fSpeed;
		if (fMovement >= fMaxSpeed) fMovement = fMaxSpeed;
		if (fMovement <= 0.0001) { fMovement = 0.0f; isBackward = false; }
	}
	fMovement *= fFriction; // friction has to be outside to reduce the movement to a halt

	if(isRight)
	{
		if (fAngle >= fMaxAngle) fAngle = fMaxAngle;
		fAngle *= fFriction;
		if (fAngle <= 0.001f) fAngle = 0.0f;
	}

	if (isLeft)
	{
		if (fAngle <= -fMaxAngle) fAngle = -fMaxAngle;
		fAngle *= fFriction;
		if (fAngle >= -0.001f) fAngle = 0.0f;
	}

	fCurrAngle += fAngle;

	vPos.x -= fMovement*cos(fCurrAngle * PI/180);
	vPos.z -= fMovement*sin(fCurrAngle * PI/180);

	//cout<<vPos.x<< " "<<vPos.z<<" m:"<<fMovement<<endl;
	//cout<<" isForward:"<<isForward<<" isBackward"<<isBackward<<" isLeft:"<<isLeft<<" isRight:"<<isRight<<endl;

	placeAgentOnTerrain();
}

void Agent::autonomy()
{
	// simulating erratic behaviour by randomising decisions

	// generate a random boolean value
	int r = rand() % 2;

	if(r == 0)
	{
		rotateLeft(2.0f);
	}
	else if (r == 1) // just to be explicit
	{
		rotateRight(2.0f);
	}

	// get another random value for thrust
	r = rand() % 2;
	if(r == 0)
	{
		moveForward(2.0f);
	}

	// check boundary and try not to leave!
	if (_grid->isInBoundary(vPos, 2.0f) == false)
	{
		rotateLeft(5.0f);
	}

}

void Agent::getGrid(Grid *grid)
{
	_grid = grid;
}

void Agent::getAgents(Agent **agents, int size)
{
  _noOfAgents = size;
  _agents = agents;
}

void Agent::rotateLeft(float fAngleSpeed)
{
	fAngle -= fAngleSpeed;
	isLeft = true;
	isRight = false;
}

void Agent::rotateRight(float fAngleSpeed)
{
	fAngle += fAngleSpeed;
	isRight = true;
	isLeft = false;
}

void Agent::moveForward(float speed)
{
  fSpeed = speed;
	isForward = true;
	isBackward = false;
}

void Agent::moveBackward(float speed)
{
  fSpeed = speed;

	isBackward = true;
	isForward = false;
}

void Agent::stopForward()
{
	isForward = false;
}

void Agent::stopBackward()
{
  isBackward = false;
}

void Agent::notMoving()
{
  isForward = false;
  isBackward = false;
}
void Agent::placeAgentOnTerrain()
{
	// skate on top of terrain
	float dist = _terrain->distanceToPlane(vPos);
	if (dist <= 0.0)	// the box is under the plane
		vPos.y = vPos.y + fabs(dist);
	else
		vPos.y = vPos.y - fabs(dist);
}

void Agent::getTerrain(QTTerrain *terrain)
{
	_terrain = terrain;
}

void Agent::DrawObject(float red, float green, float blue)
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
