//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Application
//	OOP + OpenGL integration
//
//  Quadtree-based Terrain with Predator, Prey, Emmitters inserted
//  No simulation in this program, only testing quadtree structures
//
//  ----------------------------------------------------------
//  How to compile:
//  note that we are now using both SDL2 and OpenGL, thus the -l for all libraries
//  we are also using multiple cpp files
//  sudo g++ -I/usr/include/ main.cpp Camera.cpp TerrainQuadTree.cpp QTTerrain.cpp AgentQuadTree.cpp Agent.cpp Predator.cpp Prey.cpp Snack.cpp Emitter.cpp Grid.cpp -o main -L/usr/lib -lSDL2 -lGL -lGLU
//
// -I define the path to the includes folder
// -L define the path to the library folder
// -l ask the compiler to use the library
//
//  ------ keyboard controls
//  ESC to quit
//	##########################################################

#include <iostream>
#include <string>
#include "OGLUtil.h"
#include "Grid.h"
#include "Camera.h"
#include "QTTerrain.h"
#include "AgentQuadTree.h"
#include "Agent.h"
#include "Predator.h"
#include "Prey.h"
#include "Snack.h"

using namespace std;

/****************************** PROTOTYPES ******************************/
void checkKeyPress();
void initOpenGL();
int setViewport( int width, int height );
void renderScene();
void DrawSquare(float xPos, float yPos, float zPos, float red, float green, float blue);

void createPlane(float scale, float height);
Vector3f calculatePlaneNormals();
float distanceToPlane(Vector3f pos);
void placeBoxOnPlane();
void drawCube(Vector3f pos, float red, float green, float blue);

void setupAmbientLight();


// ----------------------- Light Variables
GLfloat ambientLight[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat diffuseLight[] = {1.0f,1.0f, 1.0f, 1.0f,};

GLfloat matAmbient[] = {1.0f,1.0f, 1.0f, 1.0f,};
GLfloat matDiffuse[] = {1.0f,1.0f, 1.0f, 1.0f,};
GLfloat matSpecular[] = {1.0f,1.0f, 1.0f, 1.0f,};

/****************************** GLOBAL VARIABLES ******************************/
SDL_Event event;                // declare an SDL event
bool isRunning;                 // main loop state

SDL_Window* displayWindow;
SDL_Renderer* displayRenderer;
SDL_RendererInfo displayRendererInfo;

Camera *camera;     // CAMERA
Grid *grid;			    // declare grid

// background colour starts with black
float r, g, b = 0.0f;

// ----------------------- Terrain
QTTerrain *terrain;
AgentQuadTree *quadtree;

/****************************** MAIN METHOD ******************************/
int main(int argc, char**argv)
{
    cout<<"*********************** Initialising Scene Utility ***********************"<<endl;

    cout<<"*********************** Create a Terrain ***********************"<<endl;
    char heightMapFile[] = "terr512.raw";
    unsigned int terrainWidth = 512;
    unsigned int terrainLength = 512;
    float terrainHeightScale = 30.0f;
    float terrain_Scale = 15.0f;
    terrain = new QTTerrain(heightMapFile, terrainWidth, terrainLength, terrainHeightScale, terrain_Scale, NORMAL_SMOOTH);

    cout<<"*********************** Create a Agent Management Quadtree ***********************"<<endl;
    int maxLevels = 5;
    int divideThreshold = 4;
    quadtree = new AgentQuadTree(terrainWidth, terrain_Scale, maxLevels, divideThreshold);

    // quadtree->divideNode(&(quadtree->qtNodes[0]));

    cout<<"nodeType: "<<quadtree->qtNodes[0].nodeType<<endl;
    cout<<"childIndex: "<<quadtree->qtNodes[0].childIndex[0]<<endl;
    cout<<"childIndex: "<<quadtree->qtNodes[0].childIndex[1]<<endl;
    cout<<"childIndex: "<<quadtree->qtNodes[0].childIndex[2]<<endl;
    cout<<"childIndex: "<<quadtree->qtNodes[0].childIndex[3]<<endl;

    cout<<"------ testing AgentQuadTree SW"<<endl;
    quadtree->gAgents.push_back(new Predator(0, 10, 0, 10, 0.5f));
    quadtree->gAgents.push_back(new Predator(1, 11, 0, 11, 0.5f));
    quadtree->gAgents.push_back(new Predator(2, 12, 0, 12, 0.5f));
    quadtree->gAgents.push_back(new Predator(3, 13, 0, 13, 0.5f));
    quadtree->gAgents.push_back(new Predator(4, 14, 0, 14, 0.5f));
    quadtree->gAgents.push_back(new Predator(5, 15, 0, 15, 0.5f));

    int currNodeID01, currNodeID02, currNodeID03, currNodeID04, currNodeID05, currNodeID06;
    currNodeID01 = quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[0]);
    currNodeID02 = quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[1]);
    currNodeID03 = quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[2]);
    currNodeID04 = quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[3]);
    currNodeID05 = quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[4]);
    currNodeID06 = quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[5]);

    cout<<"Agents in which nodes: "<<currNodeID01<<","<<currNodeID02<<","
                                  <<currNodeID03<<","<<currNodeID04<<","
                                  <<currNodeID05<<","<<currNodeID06
                                  <<endl;

    // cout<<"------ testing AgentQuadTree NE"<<endl;
    quadtree->gAgents.push_back(new Prey(6, -10, 0, -10, 0.5f));
    quadtree->gAgents.push_back(new Prey(7, -11, 0, -11, 0.5f));
    quadtree->gAgents.push_back(new Prey(8, -40, 0, -40, 0.5f));
    quadtree->gAgents.push_back(new Prey(9, -45, 0, -45, 0.5f));
    quadtree->gAgents.push_back(new Prey(10, -42, 0, -42, 0.5f));
    quadtree->gAgents.push_back(new Prey(11, -150, 0, -150, 0.5f));
    quadtree->gAgents.push_back(new Prey(12, -152, 0, -152, 0.5f));
    quadtree->gAgents.push_back(new Prey(13, -153, 0, -153, 0.5f));
    quadtree->gAgents.push_back(new Prey(14, -154, 0, -154, 0.5f));
    quadtree->gAgents.push_back(new Prey(15, -156, 0, -156, 0.5f));

    quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[6]);
    quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[7]);
    quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[8]);
    quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[9]);
    quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[10]);
    quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[11]);
    quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[12]);
    quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[13]);
    quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[14]);
    quadtree->placeObjectInNode(quadtree->qtNodes[0], quadtree->gAgents[15]);

    quadtree->printAgentIDs();

    quadtree->reportNodeBranchIndex();

    quadtree->reportQuadTree(0);

    cout<<"-- testing AgentQuadTree RangeQuery"<<endl;
    cout<<"gAgents[0].getID(): "<<quadtree->gAgents[0]->getID()<<endl;
    vector<Object*> rQ = quadtree->rangeQuery(quadtree->gAgents[0], OBJTYPE_AGENT, 500);


    cout<<"*------ testing AgentQuadTree END"<<endl;


    cout<<"------- Cleaning Up Memory"<<endl;
    delete quadtree;
    delete terrain;
    cout<<"------- Objects in Memory ALL Destroyed"<<endl;

   return 0;
}

void setupAmbientLight()
{

	glEnable(GL_LIGHTING);			// enable lighting

	// setup ambient light
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

	// set the diffuse light
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);

	// enable light0
	glEnable(GL_LIGHT0);

	// enable material
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// set the material
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbient);
}

void checkKeyPress()
{
  // SDL events polling here
  while(SDL_PollEvent(&event) != 0)
  {
      // Close the window X
      if(event.type == SDL_QUIT) isRunning = false;

      // ** we have removed all controls for the agent now
      if ( event.type == SDL_KEYDOWN )
      {
        // if escape key is pressed
        if ( event.key.keysym.sym == SDLK_ESCAPE ) isRunning = false;

        // --------------------------- KEYDOWN HANDLER
        if ( event.key.keysym.sym == SDLK_RIGHT )
        {
          // cout<<"camera rotate right"<<endl;
          camera->rotateRight();
        }

        if ( event.key.keysym.sym == SDLK_LEFT )
        {
          // cout<<"camera rotate left"<<endl;
          camera->rotateLeft();
        }

        if ( event.key.keysym.sym == SDLK_UP )
        {
          // cout<<"camera forward"<<endl;
          camera->moveForward();
        }

        if ( event.key.keysym.sym == SDLK_DOWN )
        {
          // cout<<"camera backward"<<endl;
          camera->moveBackward();
        }
        if ( event.key.keysym.sym == SDLK_a )
        {
          // cout<<"camera ascend"<<endl;
          camera->ascend();
        }
         if ( event.key.keysym.sym == SDLK_z )
        {
          // cout<<"camera descend"<<endl;
          camera->descend();
        }
         if ( event.key.keysym.sym == SDLK_s )
        {
          // cout<<"camera pitchup"<<endl;
          camera->pitchDown();
        }
         if ( event.key.keysym.sym == SDLK_x )
        {
          // cout<<"camera pitch down"<<endl;
          camera->pitchUp();
        }
        if ( event.key.keysym.sym == SDLK_w )
        {
          terrain->setWireframe();
        }
        if ( event.key.keysym.sym == SDLK_e )
        {
          terrain->setEdgeMode();
        }

        // ---------------------------------------------------------------- INFO
         if ( event.key.keysym.sym == SDLK_h )
        {
  				terrain->getHeight(camera->getPosition());
        }
        if ( event.key.keysym.sym == SDLK_r )
        {
  				terrain->terrainQT->reportNodeBranchIndex();
  				camera->print();
        }

        // ---------------------------------------------------------------- TERRAIN VIEWING RANGE
        if ( event.key.keysym.sym == SDLK_EQUALS)
        {
  	      terrain->setViewRange(10);
  	      // cout<<"plus"<<endl;
        }
        if ( event.key.keysym.sym == SDLK_MINUS)
        {
  	      terrain->setViewRange(-10);
  	      // cout<<"minus"<<endl;
        }
      }

      // --------------------------- KEYUP HANDLER
      if (event.type == SDL_KEYUP)
      {
        // if ( event.key.keysym.sym == SDLK_UP ) creature->stopForward();
        // if ( event.key.keysym.sym == SDLK_DOWN ) creature->stopBackward();
      }

  }
}

// A general OpenGL initialization function that sets all initial parameters
void initOpenGL()
{
  cout<<"-------- Initialise OpenGL"<<endl;
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);   // This Will Clear The Background Color To Black
  glClearDepth(1.0);                      // Enables Clearing Of The Depth Buffer
  glDepthFunc(GL_LEQUAL);                   // The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);                // Enables Depth Testing
  glShadeModel(GL_SMOOTH);                // Enables Smooth Color Shading
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST ); // Nicest perspective calculations

  setupAmbientLight();
}

// Reset our viewport after a window resize
int setViewport( int width, int height )
{
    cout<<"-------- Setting OpenGL Viewport"<<endl;
    GLfloat ratio;

    // prevent divide by zero
    if ( height == 0 ) { height = 1; }

    // calculate ratio
    ratio = ( GLfloat )width / ( GLfloat )height;

    // Setup viewport
    glViewport( 0, 0, ( GLsizei )width, ( GLsizei )height );

    // specify which matrix is the current matrix
    // GL_PROJECTION means that we are applying all matrix to the PROJECTION MATRIX
    glMatrixMode(GL_PROJECTION);

    // Reset The Projection Matrix, generally done before we do anything to the matrix
    glLoadIdentity();

    // Calculate The Aspect Ratio Of The Window
    // gluPerspective(	GLdouble fovy, GLdouble aspect, GLdouble zNear, GLdouble zFar);
    //gluPerspective(45.0f, ratio, 0.1f, 100.0f);
    gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 5000.0f);    // Calculate The Aspect Ratio Of The Window

    // use glu function to set a camera looking at
    // void gluLookAt(	GLdouble eyeX, GLdouble eyeY, GLdouble eyeZ,
   	//                  GLdouble centerX, GLdouble centerY, GLdouble centerZ,
   	//                  GLdouble upX, GLdouble upY, GLdouble upZ);
    // the units below essentially looks down on a map view
    // putting the eye 50 units above the origin
    // looking at target at -10 units below the origin (looking down)
    // up vector is Y

    // gluLookAt(0.0f, 80.0f, 0.0f, 0.0f, -20.0f, -0.01f, 0.0f, 1.0f, 0.0f);
    gluLookAt(camera->x, camera->y, camera->z, camera->tx, camera->ty, camera->tz, 0.0f, 1.0f, 0.0f);

    // now switch to the MODELVIEW MATRIX so that we can control (transform)
    // everything we draw (rectangles, etc.)
    glMatrixMode(GL_MODELVIEW);

    /* Reset The View */
    // glLoadIdentity();

    return 1;
}

void renderScene()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen | depth buffer
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


  /* ------------ START DRAW ALL OBJECTS HERE */

	// DrawSquare(0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f); // white

  /* ------------ END DRAW ALL OBJECTS HERE */

}
