//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented Class Integrating OpenGL
//
//  A Quadtree class managing the data structures for Agent-Interaction
//
//	##########################################################

/****************************** INCLUDES ******************************/
#ifndef AGENTQUADTREE_H
#define AGENTQUADTREE_H

//#include "OGLUtil.h"
#include "stdlib.h"
#include "math.h"
#include <time.h>
#include "Vector3f.h"
#include <vector> 		// lists and collection
#include "Object.h"
#include "Agent.h"
#include "Emitter.h"

// enum NODETYPE {QT_NODE, QT_LEAF};	      // for determining whether the node is leaf

struct AGENTQUADTREENODE
{
	int nodeType;										// node or leaf enum
	int ID;													// my unique ID
	int parentID;										// for identifying parent
	float top, bottom, left, right;	// boundary
	float width, height;						// width and height of the node (bounding box size)
	Vector3f position;							// a position for comparing distance between this node with camera pos
	bool visible;										// visible nodes are where agents have traversed before

	// the division level: width/divs = LevelDivs
	// first level is 32/2^1 = 16, 16 is the division related to the index to terrainData[x][y]
	int layerID;

	// the nodes has 4 quadrants (branches), their array index is stored in this variable
	int childIndex[4];

	// For polymorphism
	// <Object*> stores pointers to derived class from globals: gAgents and gEmitters
  vector<Object*> agents;			// all agents are stored here
  vector<Object*> emitters;		// all local environment emitters are stored here
};

class AgentQuadTree
{
private:
	unsigned int leafNodesIndex; // the indices of the leaf nodes
	float minSizeOfQuad;		     // the size of the lowest level quads (leaf boundary size)
	int _divideThreshold;        // how many agents in the node to divide further
  int _maxLevels;              // maximum number of levels to divide into



public:
	AgentQuadTree();
	AgentQuadTree(float terrainWidthHeight, float terrain_Scale, int maxLevels, int divideThreshold);
	~AgentQuadTree();

	// global agents and emitters storing actual objects
	// AGENTQUADTREENODE nodes agents and emitters are pointers to the globals
	vector<Object*> gAgents;		// global agent list
	vector<Object*> gEmitters;	// global emitter list

	int nodeIndex;
	unsigned int nodeCount;
	unsigned int nodeSize;							// the number of nodes calculated from _level
	//AGENTQUADTREENODE *qtNodeArray;
	vector<AGENTQUADTREENODE> qtNodes;

	unsigned int calculateNodeSize(unsigned int _level);								// how many nodes in number of _level
	float calculateMinQuadSize(float _width, unsigned int _level);			// for determining when recursion should stop
	void resetNodeVisibility();																					// reset node visibility

  bool divideNode(AGENTQUADTREENODE *thisNode);
  int getArrayIndexOfNode(int nodeID);
  int placeObjectInNode(AGENTQUADTREENODE &parentNode, Object *obj);
  int inWhichNodeBoundary(AGENTQUADTREENODE parentNode, Vector3f pos);
	bool objectOffBoundary(Vector3f pos, float top, float bottom, float left, float right);
	// vector<Object*> getAgents();
	//int[] getNodeIndices(int nodeID);
	vector<Object*> rangeQuery(Object *obj, OBJECTTYPE objectType ,float radius);

  // void createQuads(AGENTQUADTREENODE thisNode);

	

  void reportNodeBranchIndex();	// reporting node and child visibility
	void reportQuadTree(int nodeID);				// report tree structure
	void printAgentIDs();					// report agentIDs in each node
};

#endif
