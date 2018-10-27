//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class Integrating OpenGL
//
//  A Quadtree class managing the data structures for terrain system
//  Quadtree terrain rendering class: 'QTTerrain.h'
//
//	##########################################################

/****************************** INCLUDES ******************************/
#ifndef TERRAINQUADTREE_H
#define TERRAINQUADTREE_H

//#include "OGLUtil.h"
#include "stdlib.h"
#include "math.h"
#include "Vector3f.h"

struct VERTICEINDEXINFO { int x, z; };	// vertice index mapping node vertices to indices in terrainData[x][z]
enum NODETYPE {QT_NODE, QT_LEAF};	// for determining whether the node is leaf

struct TERRAINQUADTREENODE
{
	int nodeType;								// node or leaf enum
	int ID;											// my unique ID
	int parentID;								// for identifying parent
	float top, bottom, left, right;	// boundary
	float width, height;					// width and height of the node (bounding box size)
	Vector3f position;						// a position for comparing distance between this node with camera pos
	bool visible;								// is this node visible for drawing?


	//float terrainWidth;			// a permanent width for calculating

	// the nodes has 4 quadrants (branches), their array index is stored in this variable
	unsigned int branchIndex[4];

	// a node has 9 vertices, 3 on each sides 3x3=9
	// this amounts to 4 cells
	VERTICEINDEXINFO verticeIndex[3][3];

	// the division level: width/divs = LevelDivs
	// first level is 32/2^1 = 16, 16 is the division related to the index to terrainData[x][y]
	int layerID;

	// the variables tells the next node on where to start the verticeIndex from
	int vInitX, vInitZ;
};

class TerrainQuadTree
{
private:
	unsigned int leafNodesIndex;	// the indices of the leaf nodes
	float minSizeOfQuad;		// the size of the lowest level quads (leaf boundary size)
	//int qt_level;				// quadtree level
public:
	TerrainQuadTree();
	TerrainQuadTree(float _top, float _bottom, float _left, float _right, unsigned int _vertX, unsigned int _vertZ, unsigned int _level);
	~TerrainQuadTree();

	unsigned int vertX;
	unsigned int vertZ;
	unsigned int nodeSize;		// the number of nodes calculated from _level
	TERRAINQUADTREENODE *qtNodeArray;

	unsigned int calculateNodeSize(unsigned int _level);					// how many nodes in number of _level
	float calculateMinQuadSize(float _width, unsigned int _level);			// for determining when recursion should stop
	void createQuadTree(TERRAINQUADTREENODE &thisNode);				// create quad tree
	void adjustVerticeIndex();										// adjust verticeIndex so that the last one is not 32
	void resetNodeVisibility();										// reset node visibility
	void testRenderable(TERRAINQUADTREENODE &parentNode, Vector3f pos, float range);	// cam position and chunked LOD
	void reportNodeBranchIndex();									// reporter
};

#endif
