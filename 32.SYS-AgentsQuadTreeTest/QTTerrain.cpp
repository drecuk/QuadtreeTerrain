//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class Integrating OpenGL
//
//  A Quadtree terrain class using 2D arrays to render a
//  terrain managed by procedural quadtree terrain system
//  Quadtree terrain management: 'TerrainQuadTree.h'
//
//	##########################################################

#include "QTTerrain.h"
using namespace std;

SDL_Surface *surface;
GLuint texture;
GLenum textureFormat;
GLint  nOfColors;

int normalsFlag = NORMAL_SMOOTH;

// LOAD RAW FILE
QTTerrain::QTTerrain(char *terrainFilename, const int width, const int length, float scaleH, float scale, int _normalsFlag)
{
	cout<<"---------------------------------->> Creating Terrain"<<endl;
	_wireFrame = false;


	cout<<">> Initialising 2D <Vector> terrainData"<<endl;
	for(int x = 0; x <= width+1; x++)
	{
		terrainData.push_back( vector<Vector3f>() );
		for(int z = 0; z <= length+1; z++)
		{
			//terrainData[x].push_back(Vector3f(x, 0.0f, z));
			terrainData[x].push_back(Vector3f(0.0f, 0.0f, 0.0f));

			// print out the 2D arrays
			//terrainData[x][z].print();
		}
	}


	// open the terrain texture heightmap file
	cout<<">> Opening heightField: "<<terrainFilename<<endl;
	theFile = fopen(terrainFilename, "rb");  // rb means reading in data as binary (not writing out)

	// this reads only RAW files
	// read in the data from "theFile" into "heightField"	from the start of the file "1"
	// until "width*height" then stop
	// fread(pointer to file, size per read (1 byte), size of file, the file);
	fread(heightField, 1, width * length, theFile);
	fclose(theFile); // close the file after reading
	theFile = NULL;
	//printTerrainData(); // print out the file

	// width and height of terrain (also vertex grids)
	dWidth = width;
	dHeight = length;

	// scaling factor
	scaleHeight = scaleH;
	terrainScale = scale;

	// adjustment for setting terrain centre at origin
	adjFromOrig = (terrainScale*dWidth)/2;

	// set terrain boundary
	boundary.top = -adjFromOrig;
	boundary.bottom = adjFromOrig;
	boundary.left = -adjFromOrig;
	boundary.right = adjFromOrig;

	// Load texture
	char texFile[] = "green.bmp";
	LoadTexture(texFile);


	generateTerrainPoints();
	calculateCellBoundary();

	//cout<<"@@@@@@@@@@@@@@@@ "<<terrainData[31][31].x<<" "<<terrainData[31][31].y<<" "<<terrainData[31][31].z<<endl;

	// calculate the terrain normals
	normalsFlag = _normalsFlag;	// flag assigned to global variable
	calculateNormals(normalsFlag);

	// generate QuadTree-based Chunked LOD
	terrainQT = new TerrainQuadTree(boundary.top, boundary.bottom, boundary.left, boundary.right,
												dWidth, dHeight, 7);

	// set terrain quadtree view range
	viewRange = (width*scale)/2;
	cout<<"----- Terrain View Range: "<<viewRange<<endl;

	//terrainQT->testRenderable(terrainQT->qtNodeArray[0], Vector3f(66.2504, 76.7355, -41.5497), 100.0f);

	/*
	cout<<"node [0] is:: "<<terrainQT->qtNodeArray[0].visible<<endl;
	cout<<"node [1] is:: "<<terrainQT->qtNodeArray[1].visible<<endl;
	cout<<"node [2] is:: "<<terrainQT->qtNodeArray[2].visible<<endl;
	cout<<"node [3] is:: "<<terrainQT->qtNodeArray[3].visible<<endl;
	cout<<"node [16] is:: "<<terrainQT->qtNodeArray[16].visible<<endl;

	for(int i=0; i<terrainQT->nodeSize;i++)
	{
		cout<<"node ["<<i<<"] "<<terrainQT->qtNodeArray[i].visible<<endl;
	}
	*/

	//printTerrainData();
}

QTTerrain::~QTTerrain()
{
	//delete terrainData;
	glDeleteTextures( 1, &texture );
	cout<<">> Textures deleted!"<<endl;

	// this is important!!! Can it be freed from within QTTerrainQuadTree.cpp??
	// free(terrainQT->qtNodeArray);

	delete terrainQT;
	cout<<">> QuadTree structure memory freed!"<<endl;

/*
	// cleaning up terrain memory
	for(int i = 0; i < dWidth; i++)
		delete [] terrainData[i];		// matches terrain[i] = new Vector3f[cols];

	delete[] terrainData;	// matches terrain = new Vector3f*[rows];
	terrainData = 0;
	cout<<">> terrainData memory freed!"<<endl;
*/

}

void QTTerrain::LoadTexture(char *textureFile)
{
	cout<<">> Loading terrain textures..."<<endl;

	glEnable( GL_TEXTURE_2D );	// enable 2D texture
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); // env

	nOfColors = 3;

	if( surface = SDL_LoadBMP(textureFile) )
	{

		// Have OpenGL generate a texture object handle for us
		glGenTextures( 1, &texture );

		// Bind the texture object
		glBindTexture( GL_TEXTURE_2D, texture );

		/* Next we create the actual texture. The following line tells OpenGL the texture will be a
		2D texture (GL_TEXTURE_2D). Zero represents the images level of detail, this is usually
		left at zero. Three is the number of data components. Because the image is made up of red
		data, green data and blue data, there are three components. surface->w is the
		width of the texture. If you know the width, you can put it here, but it's easier to let the
		computer figure it out for you. surface->h is the height of the texture. zero is the
		border. It's usually left at zero. GL_RGB tells OpenGL the image data we are using is made up
		of red, green and blue data in that order. GL_UNSIGNED_BYTE means the data that makes up
		the image is made up of unsigned bytes, and finally... surface->pixels tells OpenGL where
		to get the texture data from. In this case it points to the data stored in the TextureImage[0] record.*/

		// Edit the texture object's image data using the information SDL_Surface gives us
		glTexImage2D( GL_TEXTURE_2D, 0, nOfColors, surface->w, surface->h, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels );

		/*
		The next two lines tell OpenGL what type of filtering to use when the image is larger (GL_TEXTURE_MAG_FILTER)
		or stretched on the screen than the original texture, or when it's smaller (GL_TEXTURE_MIN_FILTER) on the screen
		than the actual texture. I usually use GL_LINEAR for both. This makes the texture look smooth way in the distance,
		and when it's up close to the screen. Using GL_LINEAR requires alot of work from the processor/video card, so if your
		system is slow, you might want to use GL_NEAREST. A texture that's filtered with GL_NEAREST will appear blocky when
		it's stretched. You can also try a combination of both. Make it filter things up close, but not things in the distance.
		*/
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Linear Filtering
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Linear Filtering
	}
	else
	{
		printf("SDL could not load image.bmp: %s\n", SDL_GetError());
		//SDL_Quit();
	}

	// Free the SDL_Surface only if it was successfully created
	if ( surface ) {
		SDL_FreeSurface( surface );
	}
}

// LOAD BMP FILE
/*
QTTerrain::QTTerrain(char *terrainFilename, char *TerrainTexFilename, char *waterTexFilename)
{
	imageData = QTTerrain::LoadBitmapFile(TerrainTexFilename, &bitmapInfoHeader);
	QTTerrain::generateTerrainData();
	QTTerrain::LoadTextures(TerrainTexFilename, waterTexFilename);

}
*/

void QTTerrain::printTerrainData() // print out the file
{
	cout<<">> Print Terrain Data Points"<<endl;

	// now print the file out
	for(int x=0; x < dWidth; x++)		// x
	{
		for(int z=0; z<dHeight; z++)	// z
		{
			//printf("%d, %d, %d\n", x, z, heightField[x][z]);
			//cout<<x<<" "<<z<<" "<<(float)heightField[x][z]<<" ";
			if ((float)heightField[x][z] == 0)
				cout<<"nil...   ";
			else
				cout<<(float)heightField[x][z]<<" ";
		}
		cout<<" \n"<<endl;
	}

}

void QTTerrain::render(Vector3f cameraPos)
{
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);

	GLfloat mShininess[] = {8};

	GLfloat DiffuseMaterial[] = {1.0, 1.0, 1.0};
	GLfloat AmbientMaterial[] = {0.5, 0.5, 0.5};
	GLfloat SpecularMaterial[] = {1.0, 1.0, 1.0};

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, DiffuseMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, AmbientMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, SpecularMaterial);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mShininess);

	// adjustments to align the terrain origin to the world origin
	float maxSize = dWidth*terrainScale;
	float ORG = (maxSize)/2;

	// for texture coordinates

	// update quadtree LOD based on Camera position
	terrainQT->resetNodeVisibility();
	terrainQT->testRenderable(terrainQT->qtNodeArray[0],
		Vector3f(cameraPos.x, cameraPos.y, cameraPos.z), viewRange);

	//cout<<terrainData[511][0].x<<" "<<terrainData[511][0].y<<" "<<terrainData[511][0].z<<endl;
	glPushMatrix();
		// Reset the matrix (loading identity)
		matPos.identity();
		matPos.translate(0, 0, 0);
		// load position matrix and add rotation
		//glLoadMatrixf(matPos.matrix);
		glMultMatrixf(matPos.matrix);


	// ----------------------------------------------------->> draw QUADTREE NODES
	int vX;
	int vZ;
	// run through all the nodes and test if each node, based on the camera position
	// is visible, if it is, render it.
	for(int i=0; i<terrainQT->nodeSize; i++)
	{
		//cout<<"node ["<<i<<"] "<<terrainQT->qtNodeArray[i].visible<<endl;

		// if the node is visible, draw it's vertices
		if (terrainQT->qtNodeArray[i].visible == true)
		{
			// cout<<"NODE:"<<i<<" is visible"<<endl;
			// cout<<"node ["<<i<<"] "<<terrainQT->qtNodeArray[i].visible<<" | cam:"<<cameraPos.x<<" "<<cameraPos.z<<endl;
			// grap hold of the verticeIndex and map it to terrainData[x][z]
			// cout<<"VERTICES ----"<<endl;
			const int quadSize = 3-1;

			// -------------------------- DRAW SURFACE
			if(_wireFrame)
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			else
				glPolygonMode(GL_FRONT, GL_FILL);

			glLineWidth(0.1f);
			glColor3f(1.0f, 1.0f, 1.0f);		// set colour
			for(int x=0; x<quadSize; x++)
			{
				glBegin(GL_TRIANGLE_STRIP);	// this needs to be in the first loop so that the 'strip' is drawn properly
				for(int z=0; z<quadSize; z++)
				{
					vX = terrainQT->qtNodeArray[i].verticeIndex[x][z].x;
					vZ = terrainQT->qtNodeArray[i].verticeIndex[x][z].z;
					// cout<<"vertex 0: "<<vX<<" "<<vZ<<endl;

					glNormal3f(terrainNormals[vX][vZ].x, terrainNormals[vX][vZ].y, terrainNormals[vX][vZ].z);
					glVertex3f(terrainData[vX][vZ].x, terrainData[vX][vZ].y, terrainData[vX][vZ].z);

					vX = terrainQT->qtNodeArray[i].verticeIndex[x][z+1].x;
					vZ = terrainQT->qtNodeArray[i].verticeIndex[x][z+1].z;
					// cout<<"vertex 1: "<<vX<<" "<<vZ<<endl;

					glNormal3f(terrainNormals[vX][vZ].x, terrainNormals[vX][vZ].y, terrainNormals[vX][vZ].z);
					glVertex3f(terrainData[vX][vZ].x, terrainData[vX][vZ].y, terrainData[vX][vZ].z);

					vX = terrainQT->qtNodeArray[i].verticeIndex[x+1][z].x;
					vZ = terrainQT->qtNodeArray[i].verticeIndex[x+1][z].z;
					// cout<<"vertex 2: "<<vX<<" "<<vZ<<endl;

					//cout<<"normals accessing vertex 2: "<<vX<<" "<<vZ<<endl;
					glNormal3f(terrainNormals[vX][vZ].x, terrainNormals[vX][vZ].y, terrainNormals[vX][vZ].z);
					//cout<<"terrainData accessing vertex 2: "<<vX<<" "<<vZ<<endl;
					glVertex3f(terrainData[vX][vZ].x, terrainData[vX][vZ].y, terrainData[vX][vZ].z);

					// cout<<"before vertex 3: "<<vX<<" "<<vZ<<endl;
					vX = terrainQT->qtNodeArray[i].verticeIndex[x+1][z+1].x;
					vZ = terrainQT->qtNodeArray[i].verticeIndex[x+1][z+1].z;
					// cout<<"vertex 3: "<<vX<<" "<<vZ<<endl;

					// cout<<"--normals 3: "<<vX<<" "<<vZ<<endl;
					glNormal3f(terrainNormals[vX][vZ].x, terrainNormals[vX][vZ].y, terrainNormals[vX][vZ].z);
					// cout<<"--terrainData 3: "<<vX<<" "<<vZ<<endl;
					glVertex3f(terrainData[vX][vZ].x, terrainData[vX][vZ].y, terrainData[vX][vZ].z);
				}
				// cout<<"VERTICES ---- END"<<endl;
				// cout<<"..glEnd"<<endl;
				glEnd();
			}

			if(_edgemode)
			{
				glColor3f(0.0f, 0.0f, 0.0f);		// set colour
				glLineWidth(0.5f);
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				// -------------------------- DRAW EDGE
				for(int x=0; x<quadSize; x++)
				{
					glBegin(GL_TRIANGLE_STRIP);	// this needs to be in the first loop so that the 'strip' is drawn properly
					for(int z=0; z<quadSize; z++)
					{
						vX = terrainQT->qtNodeArray[i].verticeIndex[x][z].x;
						vZ = terrainQT->qtNodeArray[i].verticeIndex[x][z].z;
						// cout<<"vertex 0: "<<vX<<" "<<vZ<<endl;

						glNormal3f(terrainNormals[vX][vZ].x, terrainNormals[vX][vZ].y, terrainNormals[vX][vZ].z);
						glVertex3f(terrainData[vX][vZ].x, terrainData[vX][vZ].y, terrainData[vX][vZ].z);

						vX = terrainQT->qtNodeArray[i].verticeIndex[x][z+1].x;
						vZ = terrainQT->qtNodeArray[i].verticeIndex[x][z+1].z;
						// cout<<"vertex 1: "<<vX<<" "<<vZ<<endl;

						glNormal3f(terrainNormals[vX][vZ].x, terrainNormals[vX][vZ].y, terrainNormals[vX][vZ].z);
						glVertex3f(terrainData[vX][vZ].x, terrainData[vX][vZ].y, terrainData[vX][vZ].z);

						vX = terrainQT->qtNodeArray[i].verticeIndex[x+1][z].x;
						vZ = terrainQT->qtNodeArray[i].verticeIndex[x+1][z].z;
						// cout<<"vertex 2: "<<vX<<" "<<vZ<<endl;

						glNormal3f(terrainNormals[vX][vZ].x, terrainNormals[vX][vZ].y, terrainNormals[vX][vZ].z);
						glVertex3f(terrainData[vX][vZ].x, terrainData[vX][vZ].y, terrainData[vX][vZ].z);

						vX = terrainQT->qtNodeArray[i].verticeIndex[x+1][z+1].x;
						vZ = terrainQT->qtNodeArray[i].verticeIndex[x+1][z+1].z;
						// cout<<"vertex 3: "<<vX<<" "<<vZ<<endl;

						glNormal3f(terrainNormals[vX][vZ].x, terrainNormals[vX][vZ].y, terrainNormals[vX][vZ].z);
						glVertex3f(terrainData[vX][vZ].x, terrainData[vX][vZ].y, terrainData[vX][vZ].z);
					}
					// cout<<"VERTICES ---- END"<<endl;

					// cout<<"..glEnd"<<endl;
					glEnd();
				}
			}

		}
	}
	glPopMatrix();
}

// update is not needed
void QTTerrain::update() { }

// loop through the x and z (vertices) with heightField points as y
// store each point in the terrainData 2D array.
void QTTerrain::generateTerrainPoints()
{
	cout<<">> Generating Terrain Points..."<<endl;
	// adjustments to align the terrain origin to the world origin

	for(int x=0; x<dWidth; x++)					// x
	{
		//cout<<"----  x :: "<<x<<"---------------"<<endl;
		for(int z=0; z<dHeight; z++)					// z
		{
			// store vector of each point, build row x at col z first
			terrainData[x][z] = Vector3f(	x*terrainScale - adjFromOrig,
																		(heightField[x][z] / 255.0f) * scaleHeight * terrainScale,	// y from height map
																		z*terrainScale - adjFromOrig
																	);

			//cout<<x<<" "<<" "<<z<<endl;
			// cout<<"terrainData----------["<<terrainData[x][z].x<<" "<<terrainData[x][z].y<<" "<<terrainData[x][z].z<<"]"<<endl;
		}

	}
	cout<<">> Terrain Points Generated Successfully"<<endl;
}

// this calculates each cell's boundary (each cell is made up of 4 vertices)
// the rationale for this is so that we can determine which 3 vertices to calculate the normals
// for getting the height map of the terrain
void QTTerrain::calculateCellBoundary()
{
	cout<<">> Calculating cell boundary..."<<endl;

	for(int x=0; x < dWidth; x++)								// x
	{
		//cout<<"----  x :: "<<x<<"---------------"<<endl;
		for(int z=0; z<dHeight; z++)								// z
		{
			cellinfo[x][z].top = z * terrainScale - adjFromOrig;			// top
			cellinfo[x][z].bottom = (z+1) * terrainScale - adjFromOrig;	// bottom
			cellinfo[x][z].left = x * terrainScale - adjFromOrig;			// left
			cellinfo[x][z].right = (x+1) * terrainScale - adjFromOrig;		// right

			//cout<<"z:"<<z<<"    top:"<<cellinfo[x][z].top<<" bottom:"<<cellinfo[x][z].bottom<<" left:"<<cellinfo[x][z].left<<" right:"<<cellinfo[x][z].right<<endl;
		}
	}

}

float QTTerrain::getHeight(Vector3f pos)
{
	float terrainHeight = 0.0f;

	// check if the position is within the terrain boundary
	if(withinBoundary(pos, boundary))
	{

		// check the distance to the plane
		float dist = distanceToPlane(pos);

		// now calculate the height of the terrain
		if (dist <= 0.0)						// the pos is under the plane
			terrainHeight = pos.y + fabs(dist);
		else
			terrainHeight = pos.y - fabs(dist);

		//cout<<"position is within terrain boundary: "<<terrainHeight<<endl;
	}
	else
	{
		//cout<<"position is NOT within terrain boundary"<<endl;
	}
	// - (-scaleHeight*10)+scaleHeight
	return terrainHeight;
}

void QTTerrain::posToArrayIndex(Vector3f &pos, int &inX, int &inZ)
{
	// precalculate the halfWidth and halfHeight
	float halfWidth = dWidth/2;
	float halfHeight = dHeight/2;

	// dereference inX and inZ
	// convert the position to the index of the terrainData[x][z]
	// "pos.x/adjFromOrig" is the ratio of the pos in relation to the terrain.
	inX = floor((pos.x/adjFromOrig) * halfWidth + halfWidth);
	inZ = floor((pos.z/adjFromOrig) * halfHeight + halfHeight);
}

bool QTTerrain::withinBoundary(Vector3f pos, CELLINFO bounds)
{
	if ((pos.x > bounds.left) && (pos.x < bounds.right))
		if ((pos.z > bounds.top) && (pos.z < bounds.bottom))
			return true;
		else
			return false;
}

float QTTerrain::distanceToPlane(Vector3f pos)
{
	// plane equation = ax + by + cz + d = 0
	float dist = 0.0f;

	// get the normals of the plane where pos is
	// this requires conversion of posToArrayIndex
	int inX;
	int inZ;
  Vector3f faceNormal;

	posToArrayIndex(pos, inX, inZ);
	// cout<<"CELL["<<inX<<"]["<<inZ<<"] T:"<<cellinfo[inX][inZ].top<<" B:"<<cellinfo[inX][inZ].bottom<<" L:"<<cellinfo[inX][inZ].left<<" R:"<<cellinfo[inX][inZ].right<<endl;

  // // which triangle on a plane is the pos on?
  bool isAbove = Vector3f::isAboveLine(
    Vector3f(terrainData[inX][inZ+1].x, terrainData[inX][inZ+1].y, terrainData[inX][inZ+1].z),
    Vector3f(terrainData[inX+1][inZ].x, terrainData[inX+1][inZ].y, terrainData[inX+1][inZ].z),
    pos);

  float D;
  if(isAbove) // top triangle
  {
	  // calculate the normals for the top pair of triangle
    faceNormal = calculateFaceNormal(terrainData[inX][inZ], terrainData[inX][inZ+1], terrainData[inX+1][inZ]);

    // dot product of plane normal and plane position
  	D = faceNormal.dotProduct(terrainData[inX][inZ]);
    //faceNormal.print();
  }
  else  // bottom triangle
  {
    faceNormal = calculateFaceNormal(terrainData[inX+1][inZ+1], terrainData[inX+1][inZ], terrainData[inX][inZ+1]);
    //faceNormal = calculateFaceNormal(terrainData[inX+1][inZ], terrainData[inX][inZ+1], terrainData[inX+1][inZ+1]);

    // dot product of plane normal and plane position
  	D = faceNormal.dotProduct(terrainData[inX+1][inZ+1]);
  }
	// cout<<"D of plane:"<<D<<endl;

	// test the pos against the plane normals
	dist = faceNormal.dotProduct(pos) - D;
	// cout<<"The cube in relation to the plane is:"<<dist<<endl;

	return  dist;
}

Vector3f QTTerrain::calculateFaceNormal(Vector3f p0, Vector3f p1, Vector3f p2)
{
	// ------------------------------------------------ calculate normals

	// get vectors between points
	Vector3f vA = p1-p0;
	Vector3f vB = p2-p0;

	// cross product of vectors
	Vector3f vN = vA.crossProduct(vB);
	vN.normalise();

	//cout<<"---------------------->> Plane Normal Vector: ";
	//vN.print();

	return vN;
}

void QTTerrain::calculateNormals(int flag)
{
	Vector3f vN;	// the normal vector for each vertex

	if(flag == NORMAL_FLAT)
	{
		cout<<">> Calculating Terrain Normals: FLAT..."<<endl;

		// loop through all vertices
		for(int x=0; x < dWidth; x++)		// x
		{
			for(int z=0; z<dWidth; z++)	// z
			{
				// get the 3 points for computing the 2 vectors
				Vector3f p0 = Vector3f(terrainData[x][z].x, terrainData[x][z].y, terrainData[x][z].z);			// original point
				Vector3f p1 = Vector3f(terrainData[x+1][z].x, terrainData[x+1][z].y, terrainData[x+1][z].z);	// the x line
				Vector3f p2 = Vector3f(terrainData[x][z+1].x, terrainData[x][z+1].y, terrainData[x][z+1].z);	// the z line

				// compute the 2 vectors
				Vector3f vA = p1 - p0;	// x line vector
				Vector3f vB = p2 - p0;	// z line vector

				// get the crossproduct of the two vectors vA and vB
				vN = vA.crossProduct(vB);

				// normalise the vector vN
				vN.normalise();

				//cout<<"vN:"<<vN.x<<" "<<vN.y<<" "<<vN.z<<endl;

				// assign the normal to terrainNormal array
				terrainNormals[x][z] = vN;
			}
		}
	}
	else if (flag == NORMAL_SMOOTH)
	{
		/**** Calculate Average of 4 quad normals for the vertex ****/
		cout<<">> Calculating Terrain Normals: SMOOTH..."<<endl;

		// loop through all vertices
		for(int x=0; x < dWidth; x++)			// x
		{
			for(int z=0; z<dHeight; z++)		// z
			{
				Vector3f pOr = Vector3f(terrainData[x][z].x, terrainData[x][z].y, terrainData[x][z].z);			// original point

				if ((x==0) && (z==0))	// origin (upper left)
				{
					// only East and South Point is computed
					//cout<<"Origin: x="<<x<<" z="<<z<<endl;

					Vector3f pE = Vector3f(terrainData[x+1][z].x, terrainData[x+1][z].y, terrainData[x+1][z].z);		// east point
					Vector3f pS = Vector3f(terrainData[x][z+1].x, terrainData[x][z+1].y, terrainData[x][z+1].z);		// south point

					// compute the 2 vectors
					Vector3f vE =	pE - pOr;	// east vector
					Vector3f vS = pS - pOr;	// south vector

					// get the crossproduct of the two vectors vE and vS
					vN = vE.crossProduct(vS);

					// normalise the vector vN
					vN.normalise();

					//cout<<"vN:"<<vN.x<<" "<<vN.y<<" "<<vN.z<<endl;

					// assign the normal to terrainNormal array
					terrainNormals[x][z] = -vN;

				}
				else if((x==0) && (z==dHeight))	// bottom left
				{
					// only North and East Point
					//cout<<"Bottom Left: x="<<x<<" z="<<z<<endl;

					Vector3f pN = Vector3f(terrainData[x][z-1].x, terrainData[x][z-1].y, terrainData[x][z-1].z);		// north point
					Vector3f pE = Vector3f(terrainData[x+1][z].x, terrainData[x+1][z].y, terrainData[x+1][z].z);		// east point

					// compute the 2 vectors
					Vector3f vE =	pE - pOr;		// east vector
					Vector3f vNr = pN - pOr;		// north vector

					// get the crossproduct of the two vectors vE and vS
					vN = vE.crossProduct(vNr);

					// normalise the vector vN
					vN.normalise();

					// assign the normal to terrainNormal array
					terrainNormals[x][z] = -vN;

				}
				else if((x==dWidth) && (z==0))	// upper right
				{
					// only compute west and south vector
					//cout<<"Upper Right: x="<<x<<" z="<<z<<endl;

					Vector3f pS = Vector3f(terrainData[x][z+1].x, terrainData[x][z+1].y, terrainData[x][z+1].z);		// south point
					Vector3f pW = Vector3f(terrainData[x-1][z].x, terrainData[x-1][z].y, terrainData[x-1][z].z);		// east point

					// compute the 2 vectors
					Vector3f vW = pW - pOr;	// west vector
					Vector3f vS = pS - pOr;		// south vector

					// get the crossproduct of the two vectors vE and vS
					vN = vW.crossProduct(vS);

					// normalise the vector vN
					vN.normalise();

					// assign the normal to terrainNormal array
					terrainNormals[x][z] = -vN;

				}
				else	if((x==dWidth) && (z==dHeight))	// bottom right
				{
					// only the north and west vector
					//cout<<"Bottom Right: x="<<x<<" z="<<z<<endl;

					Vector3f pN = Vector3f(terrainData[x][z-1].x, terrainData[x][z-1].y, terrainData[x][z-1].z);		// north point
					Vector3f pW = Vector3f(terrainData[x-1][z].x, terrainData[x-1][z].y, terrainData[x-1][z].z);		// east point

					// compute the 2 vectors
					Vector3f vNr = pN - pOr;	// north vector
					Vector3f vW = pW - pOr;	// west vector

					// get the crossproduct of the two vectors vE and vS
					vN = vNr.crossProduct(vW);

					// normalise the vector vN
					vN.normalise();

					// assign the normal to terrainNormal array
					terrainNormals[x][z] = -vN;
				}
				else if ( (x==0) && ( (z>0) && (z<dHeight) ) )	// left cell span
				{
					// only north, south, NE, east
					//cout<<"Left Cell Span: x="<<x<<" z="<<z<<endl;

					// NE quadrant
					Vector3f pN = Vector3f(terrainData[x][z-1].x, terrainData[x][z-1].y, terrainData[x][z-1].z);			// north point
					Vector3f pNE = Vector3f(terrainData[x+1][z-1].x, terrainData[x+1][z-1].y, terrainData[x+1][z-1].z);	// NE point

					Vector3f vTR = pNE - pN;	// top to the right vector (A - B is B points to A)
					Vector3f vTB = pOr - pN;	// top to the bottom vector

					Vector3f vNENorm = vTR.crossProduct(vTB);

					// SE quadrant
					Vector3f pS = Vector3f(terrainData[x][z+1].x, terrainData[x][z+1].y, terrainData[x][z+1].z);	// south point
					Vector3f pE = Vector3f(terrainData[x+1][z].x, terrainData[x+1][z].y, terrainData[x+1][z].z);	// east point

					Vector3f vOR = pE - pOr;	// origin to right vector
					Vector3f vOB = pS - pOr;	// origin to bottom vector

					Vector3f vSENorm = vOR.crossProduct(vOB);

					// sum the vector
					vN = vSENorm + vNENorm;

					// normalise the two vectors
					vN.normalise();

					// the vertex average normal is then assigned to the terrainNormals
					terrainNormals[x][z] = -vN;

				}
				else if ( (x==dWidth-1) && ( (z>0) && (z<dHeight) ) )	// **right cell span
				{
					//cout<<"Right Cell Span: x="<<x<<" z="<<z<<endl;
				}
				else if ( (z==0) && ( (x>0) && (x<dWidth) ) )	// top cell span
				{
					//cout<<"Top Cell Span: x="<<x<<" z="<<z<<endl;
				}
				else if ( (z==dHeight-1) && ( (x>0) && (x<dWidth) ) )	// **bottom cell span
				{
					//cout<<"Bottom Cell Span: x="<<x<<" z="<<z<<endl;
				}
				else	// all centre cells
				{
					//cout<<"Centre Cells: x="<<x<<" z="<<z<<endl;
					pOr = Vector3f(terrainData[x][z].x, terrainData[x][z].y, terrainData[x][z].z);			// original point
					Vector3f pN = Vector3f(terrainData[x][z-1].x, terrainData[x][z-1].y, terrainData[x][z-1].z);			// north point
					Vector3f pNE = Vector3f(terrainData[x+1][z].x, terrainData[x+1][z-1].y, terrainData[x+1][z-1].z);	// NE point
					Vector3f pE = Vector3f(terrainData[x+1][z].x, terrainData[x+1][z].y, terrainData[x+1][z].z);		// east point
					Vector3f pS = Vector3f(terrainData[x][z+1].x, terrainData[x][z+1].y, terrainData[x][z+1].z);		// south point
					Vector3f pSW = Vector3f(terrainData[x-1][z+1].x, terrainData[x-1][z+1].y, terrainData[x-1][z+1].z);	// southwest point
					Vector3f pW = Vector3f(terrainData[x-1][z].x, terrainData[x-1][z].y, terrainData[x-1][z].z);			// west point

					// NE quadrant
					Vector3f vO_N = pN - pOr;	// origin to northvector
					Vector3f vO_E = pE - pOr;	// origin to east vector

					Vector3f vNENorm = vO_N.crossProduct(vO_E);
					vNENorm.normalise();

					// NW quadrant
					Vector3f vO_W = pW - pOr;	// origin to west vector

					Vector3f vNWNorm = vO_W.crossProduct(vO_N);
					vNWNorm.normalise();

					// SE quadrant
					Vector3f vO_S = pS - pOr;	// origin to south vector

					Vector3f vSENorm = vO_S.crossProduct(vO_E);
					vSENorm.normalise();

					// SW quadrant
					Vector3f vSWNorm = vO_S.crossProduct(vO_W);
					vSWNorm.normalise();

					// sum the vector
					vN = vNENorm + vNWNorm + vSENorm +  vSWNorm;

					//vNENorm.printVector();
					//vNWNorm.printVector();
					//vSENorm.printVector();
					//vSWNorm.printVector();

					//cout<<"vN of all:";
					//vN.printVector();

					// normalise the two vectors
					vN.normalise();

					//cout<<"vN Normalised:";
					//vN.printVector();

					// the vertex average normal is then assigned to the terrainNormals
					terrainNormals[x][z] = -vN;
				}

			}
		}

		cout<<">> Terrain Normals calculated successfully"<<endl;

	}


	// Testing
	/*
	Vector3f p0 = Vector3f(0, 0, 0);			// original point
	Vector3f p1 = Vector3f(1, 5, 0);	// the x line
	Vector3f p2 = Vector3f(0, 10, 1);	// the z line

	cout<<"p0:"<<p0.x<<" "<<p0.y<<" "<<p0.z<<endl;
	cout<<"p1:"<<p1.x<<" "<<p1.y<<" "<<p1.z<<endl;
	cout<<"p2:"<<p2.x<<" "<<p2.y<<" "<<p2.z<<endl;

	Vector3f vA = p1 - p0;	// x line vector
	Vector3f vB = p2 - p0;	// z line vector

	cout<<"vA:"<<vA.x<<" "<<vA.y<<" "<<vA.z<<endl;
	cout<<"vB:"<<vB.x<<" "<<vB.y<<" "<<vB.z<<endl;

	Vector3f vN = vA.crossProduct(vB);
	cout<<"vN:"<<vN.x<<" "<<vN.y<<" "<<vN.z<<endl;
	*/



}

// LoadBitmapFile
// desc: Returns a pointer to the bitmap image of the bitmap specified
//       by filename. Also returns the bitmap header information.
//		 No support for 8-bit bitmaps.
unsigned char* QTTerrain::LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader)
{
	/*
	FILE 				*filePtr;				// the file pointer
	BITMAPFILEHEADER		bitmapFileHeader;		// bitmap file header
	unsigned char		*bitmapImage;			// bitmap image data
	int					imageIdx = 0;			// image index counter
	unsigned char		tempRGB;			// swap variable

	// open filename in "read binary" mode
	filePtr = fopen(filename, "rb");
	if (filePtr == NULL)
		return NULL;

	// read the bitmap file header
	fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	// verify that this is a bitmap by checking for the universal bitmap id
	if (bitmapFileHeader.bfType != BITMAP_ID)
	{
		fclose(filePtr);
		return NULL;
	}

	// read the bitmap information header
	fread(bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	// move file pointer to beginning of bitmap data
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// allocate enough memory for the bitmap image data
	bitmapImage = (unsigned char*)malloc(bitmapInfoHeader->biSizeImage);

	// verify memory allocation
	if (!bitmapImage)
	{
		free(bitmapImage);
		fclose(filePtr);
		return NULL;
	}

	// read in the bitmap image data
	fread(bitmapImage, 1, bitmapInfoHeader->biSizeImage, filePtr);

	// make sure bitmap image data was read
	if (bitmapImage == NULL)
	{
		fclose(filePtr);
		return NULL;
	}

	// swap the R and B values to get RGB since the bitmap color format is in BGR
	for (imageIdx = 0; imageIdx < bitmapInfoHeader->biSizeImage; imageIdx+=3)
	{
		tempRGB = bitmapImage[imageIdx];
		bitmapImage[imageIdx] = bitmapImage[imageIdx + 2];
		bitmapImage[imageIdx + 2] = tempRGB;
	}

	// close the file and return the bitmap image data
	fclose(filePtr);
	*/

	//return bitmapImage;
}


bool QTTerrain::LoadTextures(char *TerrainFilename, char *waterFilename)
{
	/*
	// load the land texture data
	landTexture = LoadBitmapFile(TerrainFilename, &landInfo);
	if (!landTexture)
		return false;

	// load the water texture data
	waterTexture = LoadBitmapFile(waterFilename, &waterInfo);
	if (!waterTexture)
		return false;

	// generate the land texture as a mipmap
	glGenTextures(1, &land);
	glBindTexture(GL_TEXTURE_2D, land);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, landInfo.biWidth, landInfo.biHeight, GL_RGB, GL_UNSIGNED_BYTE, landTexture);

	// generate the water texture as a mipmap
	glGenTextures(1, &water);
	glBindTexture(GL_TEXTURE_2D, water);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, waterInfo.biWidth, waterInfo.biHeight, GL_RGB, GL_UNSIGNED_BYTE, waterTexture);

	*/
	return true;
}

void QTTerrain::setViewRange(float value)
{
	viewRange += value;
	//cout<<"-- viewRange: "<<viewRange<<endl;
}

void QTTerrain::setWireframe()
{
	_wireFrame = !_wireFrame;
	//cout<<"wireframe: "<<_wireFrame<<endl;
}

void QTTerrain::setEdgeMode()
{
	_edgemode = !_edgemode;
	cout<<"edgemode: "<<_edgemode<<endl;
}
