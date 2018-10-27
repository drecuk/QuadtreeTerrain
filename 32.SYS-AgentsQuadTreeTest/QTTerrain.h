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

/****************************** INCLUDES ******************************/
#ifndef QTTERRAIN_H
#define QTTERRAIN_H

#include "OGLUtil.h"
#include "typedefs.h"
#include "TerrainQuadTree.h"

// BMP-------------------------------------------------------------------- START
#define BITMAP_ID	0x4D42	// the universal bitmap ID
#define MAP_X		32		// size of map along x axis
#define MAP_Z		32		// size of map along z axis
#define MAP_SCALE	20.0f	// the scale of the terrain map

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct tagBITMAPFILEHEADER {
  WORD bfType;
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER;
// BMP---------------------------------------------------------------------- END

// which cell is the current cell
struct CELLINFO
{
	float top;
	float bottom;
	float left;
	float right;
};

// for shading and normals calculation
enum { NORMAL_FLAT, NORMAL_SMOOTH };

/****************************** PROTOTYPES ******************************/
class QTTerrain
{
private:

	// BMP INFO
	// ---------------------------------------------------------------------------
	BITMAPINFOHEADER bitmapInfoHeader2;		// temp bitmap info header
	BITMAPINFOHEADER landInfo;				// land texture info header
	BITMAPINFOHEADER waterInfo;				// water
	unsigned char* 	imageData;			// map image data
	unsigned char*	landTexture;
	unsigned char*	waterTexture;
	unsigned int		land;				// land texture object
	unsigned int 		water;				// water texture object
	// terrain data points
	//float terrain[MAP_X][MAP_Z][3];
	//Vector3f terrainData[32][32];
	//Vector3f **terrainData;

  bool _wireFrame;
  bool _edgemode;


	// ---------------------------------------------------------------------------

	// RAW INFO
	// ---------------------------------------------------------------------------
	uint8_t heightField[512][512];		// the size of the heightfield[x][y]
	Vector3f terrainNormals[512][512];	// the terrain normals for each point 32*32=1024
	//Vector3f **terrainNormals;

	CELLINFO cellinfo[512][512];		// each polygon (quad) is a cell (this is its boundary)
	CELLINFO boundary;			// boundary of the entire terrain (2D bounding box)

	FILE *theFile;					// handle for the the raw file
	float scaleHeight;			// height scaling factor of terrain
	float terrainScale;			// scaling size for terrain
	float adjFromOrig;			// adjustment variable to set terrain centre at origin

	int dWidth, dHeight;		// width and height of terrain (how many pixels)
	float viewRange;				// viewing range of quadtree LOD nodes

	// ---------------------------------------------------------------------------
  // Matrix for the terrain
	Matrix4x4 matPos;
	Matrix4x4 matRot;
	Vector3f 	vPos;

public:
	QTTerrain(){}
	QTTerrain(char *terrainFilename, const int width, const int length,
              float scaleH, float scale, int normalsFlag);
	//QTTerrain(char *terrainFilename, char *TerrainTexFilename, char *waterTexFilename);
	~QTTerrain();

	// Quadtree-based Terrain (Chunked LOD)
	TerrainQuadTree *terrainQT;

  //uint8_t **heightField;					// the size of the heightfield[x][y]
	//vector<vector<Vector3f> > terrainNormals;	// the terrain normals for each point 32*32=1024
	vector<vector<Vector3f> > terrainData;		// the terrain data points row and cols

	void LoadTexture(char *textureFile);
	void printTerrainData();
	void render(Vector3f cameraPos);
	void update();
	void generateTerrainPoints();
	void calculateCellBoundary();
	float getHeight(Vector3f pos);
	void posToArrayIndex(Vector3f &pos, int &inX, int &inZ);
	bool withinBoundary(Vector3f pos, CELLINFO bounds);
	float distanceToPlane(Vector3f pos);
	Vector3f calculateFaceNormal(Vector3f p0, Vector3f p1, Vector3f p2);
	void calculateNormals(int flag);
	unsigned char *LoadBitmapFile(char *filename, BITMAPINFOHEADER *bitmapInfoHeader);
	bool LoadTextures(char *TerrainFilename, char *waterFilename);
	void setViewRange(float value);
  void setWireframe();
  void setEdgeMode();
};

#endif
