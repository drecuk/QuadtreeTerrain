//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Application
//	OOP + OpenGL integration
//
//  Quadtree-based Terrain with Predator-Prey-Snacks
//
//  ----------------------------------------------------------
//  How to compile:
//  note that we are now using both SDL2 and OpenGL, thus the -l for all libraries
//  we are also using multiple cpp files
//  sudo g++ -I/usr/include/ main.cpp Camera.cpp TerrainQuadTree.cpp QTTerrain.cpp Agent.cpp Predator.cpp Prey.cpp Snack.cpp Grid.cpp -o main -L/usr/lib -lSDL2 -lGL -lGLU
//
// -I define the path to the includes folder
// -L define the path to the library folder
// -l ask the compiler to use the library
//
//  ------ keyboard controls
//  ESC to quit
//  Arrow keys to move the camera
//  a and z to ascend and descend
//  s and x to pitch up and down
//  w and e to set wireframe mode and switch on/off mesh edges
//  + and - to set quadtree view range
//  i,j,k,l to move the agent - MoveableOnQTTerrain.h
//	##########################################################

#include <iostream>
#include <string>
#include "OGLUtil.h"
#include "Grid.h"
#include "Camera.h"
#include "QTTerrain.h"
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

    cout<<"*********************** Create a Camera ***********************"<<endl;
    camera = new Camera(Vector3f(0, 30.0f, 100.0f), Vector3f(0.0f, 0.0f, -1.0f), 0.5f, 3.0f, 20.0f);

    // set camera on top of terrain
    float cameraEyeHeight = terrain->getHeight(camera->getPosition()) + 30.0f;
    cout<<"Camera Eye Height: "<<cameraEyeHeight<<endl;
    camera->setEyeHeight(cameraEyeHeight);

    cout<<"*********************** Create a Grid ***********************"<<endl;
    //  instantiate grid
    float gridWidth = 512.0f;
    float gridLength = 512.0f;
    float gridSpacing = 16.0f;
    grid = new Grid(gridWidth*terrain_Scale, gridLength*terrain_Scale, gridSpacing);

    cout<<"*********************** Initialising Agents ***********************"<<endl;
    // instantiate n agents and assign them arbitrary speed
    // derived classes
    int noPred = 30;
    int noPrey = 40;
    int noSnack = 60;
    int agentNo = noPred + noPrey + noSnack;
    cout<<"-- Total number of agents: "<<agentNo<<endl;
    cout<<"-- Number of Predator: "<<noPred<<endl;
    cout<<"-- Number of Prey: "<<noPrey<<endl;
    cout<<"-- Number of Snacks: "<<noSnack<<endl;

    // this needs not be instantiated
    // the derived types are assigned to the array later
    Agent **agents = new Agent*[agentNo];
    // for(int i=0; i<agentNo; i++)
    //   agents[i] = new Agent(1, 0, 0, 0, 0.001f);

    Predator **predators = new Predator*[noPred];
    Prey **preys = new Prey*[noPrey];
    Snack **snacks = new Snack*[noSnack];

    // 1st Loop: create agents types first
    cout<<"----- Creating Types"<< endl;
    for(int i=0; i<agentNo; i++)
    {
      if(i<noPred) //30
      {
        // randomise location of snacks
    		int min = grid->getBottom();
    		int max = grid->getBottom() + grid->getBottom();

    		int newX = (rand()%max)-min;
    		int newZ = (rand()%max)-min;
        cout<<"creating predator "<<i<<endl;
        predators[i] = new Predator(i, newX, 0, newZ, 0.5f);
      }
      else if(i>=noPred && i<noPred+noPrey)
      {
        // randomise location of snacks
    		int min = grid->getBottom();
    		int max = grid->getBottom() + grid->getBottom();

    		int newX = (rand()%max)-min;
    		int newZ = (rand()%max)-min;
        cout<<i<<">="<<noPred<<" && "<<i<<"<"<<noPred+noPrey;
        cout<<" | creating prey "<<i<<" | actual index: "<<i-noPred<<endl;
        preys[i-noPred] = new Prey(i, newX, 0, newZ, 0.5f);
      }
      else if(i>=noPred+noPrey)
      {
        // randomise location of snacks
        int min = grid->getBottom();
    		int max = grid->getBottom() + grid->getBottom();

    		int newX = (rand()%max)-min;
    		int newZ = (rand()%max)-min;

        cout<<"creating snacks "<<i<<" | actual index: "<<i-(noPred+noPrey)<<endl;
        snacks[i-(noPred+noPrey)] = new Snack(i, newX, 0, newZ, 0.0f);
      }
    }

    cout << "----- Deriving Types" << endl;
    for(int i=0; i<agentNo; i++)
    {
      if(i<noPred)
      {
        cout<<"-- deriving predators["<<i<<"] "<<predators[i]<<endl;
        agents[i] = predators[i];   // fishes[i] is the address
        agents[i]->speciesType = PREDATOR;
      }
      else if(i>=noPred && i<noPred+noPrey)
      {
        cout<<"-- deriving preys["<<i<<"] "<<preys[i-noPred]<<endl;
        agents[i] = preys[i-noPred];
        agents[i]->speciesType = PREY;
      }
      else if(i>=noPred+noPrey)
      {
        cout<<"-- deriving snacks["<<i<<"] "<<snacks[i-(noPred+noPrey)]<<endl;
        agents[i] = snacks[i-(noPred+noPrey)];
        agents[i]->speciesType = SNACK;
        // cout<<"snack species type"<<agents[i]->speciesType<<endl;
      }
    }

    cout << "----- Getting grid and agents to be accessible to all agents" << endl;
    for(int i=0; i<agentNo; i++)
    {
      cout<<"grid "<<i<<endl;
      agents[i]->getGrid(grid);
      // cout<<"Agents"<<i<<endl;
      agents[i]->getAgents(agents, agentNo);
      // cout<<"terrain"<<i<<endl;
      agents[i]->getTerrain(terrain);
    }

    cout<<"*********************** Begin SDL OpenGL ***********************"<<endl;

    cout<<"-------- Using OpenGL 3.0 core "<<endl;
    // Use OpenGL 3.1 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // set states
    isRunning = true;

    // initialise SDL Video out
    cout<<"-------- Initialise SDL"<<endl;
    if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) {
        cout<<"Unable to initialise SDL: "<<SDL_GetError()<<endl;
        exit(1);
    }

    SDL_CreateWindowAndRenderer(1024, 786, SDL_WINDOW_OPENGL, &displayWindow, &displayRenderer);

    SDL_GetRendererInfo(displayRenderer, &displayRendererInfo);
    // Check that we have OpenGL
    if ((displayRendererInfo.flags & SDL_RENDERER_ACCELERATED) == 0 ||
        (displayRendererInfo.flags & SDL_RENDERER_TARGETTEXTURE) == 0) {
        /*TODO: Handle this. We have no render surface and not accelerated. */
    }

    // Sets title bar
    SDL_SetWindowTitle(displayWindow, "OpenGL OOP Integration");

    // init the OpenGl window
    initOpenGL();

    // setup viewport
    setViewport(1024, 786);

    // --------------------- SIMULATION BLOCK
    cout<<"------- SIMULATION BLOCK STARTED"<<endl;
    // Simulation main loop is defined here
    // note that without using GLUT, we are now able to control
    // everything which runs within the loop using our own implementation

    int frameRate = 1000 / 60;
    Uint32 timeStart = SDL_GetTicks();
    float px = 0.0f;
    while (isRunning) {
        checkKeyPress();

        // The while loop runs too quickly on most systems which can hang
        // most machines. The code below keeps the update consistent
        // increase the frameRate above to slow down the simulation
        if (SDL_GetTicks() >= timeStart + frameRate)
        {
          timeStart = SDL_GetTicks();
          //cout<<"timestart:"<<timeStart<<endl;
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear the screen | depth buffer
          glLoadIdentity();

          // ------------------ START ALL UPDATES AND RENDERING HERE
          camera->update();
            //void gluLookAt(	GLdouble eyeX, GLdouble eyeY,	GLdouble eyeZ, GLdouble centerX,GLdouble centerY,	GLdouble centerZ,	GLdouble upX,	GLdouble upY,	GLdouble upZ);
            gluLookAt(camera->x, camera->y, camera->z, camera->tx, camera->ty, camera->tz, 0.0f, 1.0f, 0.0f);

          // we need to draw the components of the world in relation
          // to the grid's matrix stack, therefore the push and pop here to
          // couple all of them together
          glPushMatrix();
            grid->render();
            terrain->render(camera->getPosition());

            // agents update
            for(int i=0; i<agentNo; i++)
            {
              // cout<<"agent "<<i<<endl;
              agents[i]->update();
              agents[i]->render();
            }
          glPopMatrix();

          // Update window with OpenGL rendering
          SDL_GL_SwapWindow(displayWindow);

          // ------------------ END ALL UPDATES AND RENDERING HERE
        }
    }

    cout<<"------- SIMULATION BLOCK ENDED"<<endl;

    // clear the screen to default
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    cout<<"------- Cleaning Up Memory"<<endl;

    cout<<"---- deleting predators"<<endl;
    // for(int i = 0; i < noPred; i++)
    //   predators[i]->~Predator();
    delete[] predators;

    cout<<"---- deleting preys"<<endl;
    // for(int i = 0; i < noPrey; i++)
    //   preys[i]->~Prey();
    delete[] preys;

    cout<<"---- deleting snacks"<<endl;
    // for(int i = 0; i < noSnack; i++)
    //   snacks[i]->~Snack();
    delete[] snacks;

    cout<<"---- deleting agents"<<endl;
    // for(int i = 0; i < agentNo; i++)
    //   delete agents[i]; // calls each ~Agent()
    delete[] agents; // delete the heap memory

    cout<<"---- deleting grid"<<endl;
    delete grid;

    cout<<"---- deleting camera"<<endl;
    delete camera;

    cout<<"---- deleting terrain"<<endl;
    delete terrain;

    // Destroy window
    SDL_DestroyWindow(displayWindow);

    displayRenderer = NULL;
    displayWindow = NULL;
    cout<<"------- Objects in Memory ALL Destroyed"<<endl;

    SDL_Quit();

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
