//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class Integrating OpenGL
//
//  This example shows how to integrate OpenGL with Objects
//  encapsulating all internal functions, essentially creating
//  a pattern of an Predator in Predator-based modelling systems
//
//	Autonomy is in the free roaming, and avoidance of boundaries
//	leaving the world, seeking and chasing after prey
//
//	* predator will never catch the prey in this version
//	See Prey.ccp eat Snacks.cpp for the eating implementation
//
//  The Predator class prototype derives from Object
//  Prototype details are decoupled in Predator.h
//  Decoupling codes make it very easy to manage programmatically
//
//	UPDATES
//	- Using Quadtree terrain
//	- If left the boundary of the landscape reset position to 0,0,0
//
//	##########################################################

#include "OGLUtil.h"
#include "Predator.h"

Predator::Predator(int _id, float origX, float origY, float origZ, float speed): Agent(_id, origX, origY, origZ, speed)
{
  // setting standard movement, rotation speed
	fAngle = 0;
	fCurrAngle = 0;
	fAngleSpeed = 0.1f;
	fMaxAngle = 5.0f;		// increased max angle now
	fSpeed = speed;
	fMaxSpeed = 2.0f;
	fMovement = 0.0f;
	fFriction = 0.99f;
	objectType = OBJTYPE_AGENT;
	
	fScale = 10.0f;

	isForward = isBackward = isRight = isLeft = false;

	vPos.x = origX;
	vPos.y = origY;
	vPos.z = origZ;

  // Predator VARIABLES
  _preyID = -1; 	// started with no prey
  _distanceToTarget = 250.0f;
  fov = -1.0f;		// -5.0 = 45 degree from angle FOV

}

Predator::~Predator()
{
  cout<<"Predator destroyed!"<<endl;
}

// void Predator::setBoundary(float top, float bottom, float left, float right)
// {
// 	_top = top;
// 	_bottom = bottom;
// 	_left = left;
// 	_right = right;
// }

void Predator::render()
{
	// ** glIdentity must be called here so that
	// the matrix transform is reset to identity matrix made to this Predator
	// so that it doesn't affect all other Predators later
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

void Predator::update()
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

void Predator::autonomy()
{
	// simulating erratic behaviour by randomising decisions

  if(_preyID == -1) // if no prey
  {
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


    seek();
  }
  else
    chase();


		// check boundary and try not to leave!
    if (_grid->isInBoundary(vPos, 2.0f) == false)
    {
			vPos.x = 0.0f;
			vPos.y = 0.0f;
			vPos.z = 0.0f;
      //rotateLeft(5.0f);
    }
}

void Predator::seek()
{
  for(int i = 0; i < _noOfAgents; i++)
  {
      Vector3f p = _agents[i]->getPosition();
      // cout<<i<<" "<<p.x<<","<<p.z<<endl;
      if(_agents[i]->speciesType == PREY)
        if(p.distance(vPos, p) < _distanceToTarget)
        {
          Vector3f visibleVec = Vector3f::vRotate2D(fCurrAngle-90, vPos, p);

					// test if within viewing angle
          if(visibleVec.z < fov)
          {
						// assign target ID if a prey is within eyesight
            _preyID = i;
            break;
          }
        }
  }
}

void Predator::chase()
{
	// get the position of the prey based on the target (preyID)
  Vector3f preyPos = _agents[_preyID]->getPosition();

  float fx = vPos.x - preyPos.x;
  float fz = vPos.z - preyPos.z;
	// float fx = preyPos.x - vPos.x;
  // float fz = preyPos.z - vPos.z;

  //float Angle = round(atan2(fz,fx)*180/PI);

  Vector3f visibleVec = Vector3f::vRotate2D(fCurrAngle-90, vPos, preyPos); // get visibility vector

	//if(p.distance(vPos, p) < 15.0f)		// within distance of 15
		if(visibleVec.z < fov)					// if within FOV
			if (visibleVec.x < -1) // if prey at left, turn left
	      rotateLeft(1.0f);
	    else if (visibleVec.x > 1) // if prey at right, turn right
	      rotateRight(1.0f);


	// lose the prey if beyond a certain range
  if (vPos.distance(vPos, preyPos) > _distanceToTarget + 5)
     _preyID = -1;

}

void Predator::DrawObject(float red, float green, float blue)
{
	glColor3f(red, green, blue);		// set colour to red
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// head pointing towards the positive x axis at the start
	glLineWidth(3.0f);
	glBegin(GL_TRIANGLES);				// draw the triangle
		// right
		glVertex3f(0.0f, 1.0f*fScale, 0.0f);
		glVertex3f(0.0f, 0.0f, 1.0f*fScale);
		glVertex3f(2.0f*fScale, 0.0f, 0.0f);	// head
		// left
		glVertex3f(0.0f, 1.0f*fScale, 0.0f);
		glVertex3f(2.0f*fScale, 0.0f, 0.0f);	// head
		glVertex3f(0.0f, 0.0f, -1.0f*fScale);
		// back
		glVertex3f(0.0f, 1.0f*fScale, 0.0f);
		glVertex3f(0.0f, 0.0f, 1.0f*fScale);
		glVertex3f(0.0f, 0.0f, -1.0f*fScale);
		// belly
		glVertex3f(0.0f, 1.0f*fScale, 0.0f);
		glVertex3f(0.0f, 0.0f, 1.0f*fScale);
		glVertex3f(0.0f, 0.0f, -1.0f*fScale);
	glEnd();
	glLineWidth(0.2f);

	// lines
	glLineWidth(0.5f);
	glBegin(GL_LINES);
		// vertical
		glVertex3f(-1.0f*fScale, 0.0f, 0.0f);
		glVertex3f(2.0f*fScale, 0.0f, 0.0f);
	glEnd();

	glBegin(GL_LINES);
		// horizontal
		glVertex3f(0.0f, 0.0f, -2.0f*fScale);
		glVertex3f(0.0f, 0.0f, 2.0f*fScale);
	glEnd();
}
