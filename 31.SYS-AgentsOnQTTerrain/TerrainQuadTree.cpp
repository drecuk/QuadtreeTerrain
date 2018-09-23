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

#include <iostream>
#include "TerrainQuadTree.h"

using namespace std;

TerrainQuadTree::TerrainQuadTree()
{
	cout<<"---------------------------------->> Creating QuadTree"<<endl;
}
// unsigned int _vertexX, unsigned int _vertexY
TerrainQuadTree::TerrainQuadTree(float _top, float _bottom, float _left, float _right,
													unsigned int _vertX, unsigned int _vertZ, unsigned int _level)
{
	cout<<"---------------------------------->> Creating QuadTree"<<endl;

	// calculate levels log4(1024) = 5
	// log(1024)/log(4) = 5
	int levels = log(pow(_bottom - _top, 2)) / log(4);
	cout<<">> Number of levels calculated from the size of the terrain: "<<levels<<endl;

	// calculate the number of nodes for memory allocation
	nodeSize = calculateNodeSize(_level);

	// calculate minimum node boundary size
	// this is for determining when a leaf node is found, for stopping further child node creation
	minSizeOfQuad = calculateMinQuadSize(_bottom - _top, _level);


	//leafNodesIndex = nodeSize - (unsigned int)pow(4, _level-1);
	//cout<<"********************* LEAF NODES STARTING FROM INDEX:"<<leafNodesIndex<<endl<<endl;

	// allocate memory for it
	qtNodeArray = (TERRAINQUADTREENODE*)malloc( sizeof(TERRAINQUADTREENODE) * nodeSize );
	
	// ----------------------------------------------------------------------->> input first node (root) details
	cout<<">> Creating First Quadtree Node..."<<endl;
	TERRAINQUADTREENODE firstNode;	// declare the first node

	// input boundaries
	firstNode.top = _top;
	firstNode.bottom = _bottom;
	firstNode.left = _left;
	firstNode.right = _right;

	// set IDs
	firstNode.ID = 0;
	firstNode.parentID = 0;

	// input layer information
	firstNode.layerID = 1;	// first layer (this is used as 4^layerID to divide the landscape)
	firstNode.vInitX = 0;		// where to start the verticeIndex X
	firstNode.vInitZ = 0;		// where to start the verticeIndex Z

	// how many vertices on x and z
	vertX = _vertX;	// 512
	vertZ = _vertZ;	// 512

	createQuadTree(firstNode);	// ****** create the quadtree structure (recursion)
	adjustVerticeIndex();

	// report the quadtree branch index
	//reportNodeBranchIndex();
}

TerrainQuadTree::~TerrainQuadTree()
{
	// for (int i=0; i<nodeSize; ++i)
  //   free(qtNodeArray[i]);
	cout<<"free(qtNodeArray)"<<endl;
	free(qtNodeArray);
	cout<<"free(qtNodeArray) SUCCESS"<<endl;
}

unsigned int TerrainQuadTree::calculateNodeSize(unsigned int _level)
{
	cout<<">> Calculating number of nodes... "<<endl;
	unsigned int numNodes = 0;

	for(int i=0; i<_level; i++)
	{
		numNodes += pow(4, i);
		cout<<"Level ["<<i<<"] total nodes now is: "<<numNodes<<endl;

	}

	cout<<"********************* TOTAL NUMBER OF NODES: "<<numNodes<<endl;
	cout<<"********************* SIZE OF EACH NODE: "<<sizeof(TERRAINQUADTREENODE)<<" BYTES"<<endl;
	cout<<"********************* TOTAL SIZE OF ALL NODES: "<<sizeof(TERRAINQUADTREENODE)*numNodes<<" BYTES"<<endl;

	return numNodes;
}

float TerrainQuadTree::calculateMinQuadSize(float _width, unsigned int _level)
{
	float size = _width;

	for(int i=0; i<_level-1; i++)
	{
		size *= 0.5;
		//cout<<size<<endl;
	}

	cout<<"********************* Smallest Quad Size (Leaf Node Size): "<<size<<" OpenGL 3D Units"<<endl;

	return size;
}

//void CQuadTree::createQuadTree(float _top, float _bottom, float _left, float _right, unsigned int parentID, unsigned int nodeID)
void TerrainQuadTree::createQuadTree(TERRAINQUADTREENODE &thisNode)
{
	static int nodeIndex;
	unsigned int theCurrentNodeType;

	// set the default visibility of the node

	// testing for leaf and assigning nodeType based on the size of the quad
	if ((thisNode.bottom - thisNode.top) == minSizeOfQuad)
	{	theCurrentNodeType = QT_LEAF;	}
	else
	{	theCurrentNodeType = QT_NODE;	}

		// ------------------------------------------------------------------------------------------------------->> CREATE THIS NODE
		//cout<<"------------------------------------------------------------------------------  node["<<thisNode.ID<<"] parent["<<thisNode.parentID<<"]"<<endl;

		// declare a pointer to the qtNodeArray's [nodeID]'s element
		TERRAINQUADTREENODE *pNode = &qtNodeArray[thisNode.ID];

		pNode->parentID = 	thisNode.parentID;	// parent id comes from the previous node's ID
		pNode->ID = 				thisNode.ID;				// this node ID is the nodeIndex passed in
		pNode->nodeType = 	theCurrentNodeType;	// assign node type
		pNode->visible =		false;							// default visibility (set later at setRenderable());

		// calculate the width and height of this node from the bounds passed in
		// note that all quadrants (child nodes) in the level has the same width and height
		pNode->width = 			thisNode.bottom - thisNode.top;
		pNode->height = 		thisNode.right - thisNode.left;

		// receives boundary parameters for this node
		pNode->top = 			thisNode.top;
		pNode->bottom = 	thisNode.bottom;
		pNode->left = 		thisNode.left;
		pNode->right = 		thisNode.right;

		// calculate central axial position of this node (centre of quad boundary)
		pNode->position.x =	((thisNode.left + thisNode.right) / 2);
		pNode->position.z =	((thisNode.top + thisNode.bottom) / 2);

		//cout<<"nodeType:: "<<pNode->nodeType<<"   | width:"<<pNode->width<<" height:"<<pNode->height<<" | top:"<<thisNode.top<<" bottom:"<<thisNode.bottom<<" left:"<<thisNode.left<<" right:"<<thisNode.right<<endl;
		//cout<<"central position: "<<pNode->position.x<<" "<<pNode->position.y<<" "<<pNode->position.z<<endl;

		// start-------------------------------------------------------------------------------------------------> PUT NODE VERTICE INDEX HERE!!!
		// calculate divs on each layer, for root layer, layerID=1
		// (pNode->width*pow(2, thisNode.layerID)/2) is to maintain the original max terrain size
		//int divs = (pNode->width*pow(2, thisNode.layerID)/2) / pow(2, thisNode.layerID);
		int divs = (vertX/pow(2, thisNode.layerID));		// -1 because the terrainData array is 0-31 elements (total 32).
		// cout<<"----- verticeIndex] layerID:"<<thisNode.layerID<<endl;
		// cout<<(pNode->width*pow(2, thisNode.layerID)/2)<<" / 2^"<<thisNode.layerID<<" = "<<divs<<" |   2^"<<thisNode.layerID<<": "<<pow(2, thisNode.layerID)<<endl;
		// 512 / 2^1 = 256 |    2^1: 2

		// construct and store vertex indices
		//cout<<"-------------------- Construct VerticeIndex"<<endl;
		for(int x=0; x<3; x++)
		{
			for(int z=0; z<3; z++)
			{
				pNode->verticeIndex[x][z].x = thisNode.vInitX + (divs * x);
				pNode->verticeIndex[x][z].z = thisNode.vInitZ + (divs * z);
				// cout<<"pNode->verticeIndex[x][z].x<< = <<thisNode.vInitX<< + (<<divs<< * <<x<<);"<<endl;
				// cout<<pNode->verticeIndex[x][z].x<<" = "<<thisNode.vInitX<<" + ("<<divs<<" * "<<x<<");"<<endl;
				// cout<<pNode->verticeIndex[x][z].z<<" = "<<thisNode.vInitZ<<" + ("<<divs<<" * "<<z<<");"<<endl;

				//cout<<"verticeIndex["<<pNode->verticeIndex[x][z].x<<" "<<pNode->verticeIndex[x][z].z<<"]  "<<endl;
			}
			// cout<<endl;
		}

		// end-------------------------------------------------------------------------------------------------> PUT NODE VERTICE INDEX HERE!!!


		// ------------------------------------------------------------------------------------------------------->> CREATE CHILD NODES
		if(theCurrentNodeType == QT_LEAF)	// if leaf, quit
		{
			//cout<<"A LEAF NODE -- STOP CREATING FURTHER CHILD"<<endl;
			return;
		}
		else														// otherwise, create its children nodes
		{
			//cout<<"A NODE -- CREATE FURTHER CHILD"<<endl;

			// PREPARE A TEMP QUADTREENODE STRUCT
			// prepare boundary variables
			TERRAINQUADTREENODE childNode;
			//float childwidth, childheight, childtop, childbottom, childleft, childright;

			// calculate width and height of every child node (parent size divided by 2, all child are the same)
			childNode.width = 		pNode->width / 2;
			childNode.height = 		pNode->height / 2;

			//  ----------------------------------------------------------------------------------------------- QUADRANT 0 (NW)
			nodeIndex++; 		// go to next index
			pNode->branchIndex[0] = nodeIndex;

			// calculate boundary
			childNode.top = 			pNode->top;
			childNode.bottom = 		pNode->top + childNode.height;
			childNode.left = 			pNode->left;
			childNode.right = 		pNode->left + childNode.width;
			childNode.parentID = 	thisNode.ID;			// parent nodeIndex
			childNode.ID = 				nodeIndex;

			childNode.layerID = 	thisNode.layerID + 1;		// the next layer now
			childNode.vInitX = 		thisNode.vInitX;			// based on parent's
			childNode.vInitZ = 		thisNode.vInitZ;			// based on parent's

			//cout<<"pNode->branchIndex[0]: "<<nodeIndex;
			//cout<<" | top:"<<childtop<<" bottom:"<<childbottom<<" left:"<<childleft<<" right:"<<childright<<endl;

			createQuadTree(childNode);

			// ----------------------------------------------------------------------------------------------- QUADRANT 1 (SW)
			nodeIndex++; 		// go to next index
			pNode->branchIndex[1] = nodeIndex;

			// calculate boundary
			childNode.top = 			pNode->bottom - childNode.height;
			childNode.bottom = 		childNode.top + childNode.height;
			childNode.left = 			pNode->left;
			childNode.right = 		pNode->left + childNode.width;
			childNode.parentID = 	thisNode.ID;			// parent nodeIndex
			childNode.ID = 				nodeIndex;

			childNode.layerID = 	thisNode.layerID + 1;		// the next layer now
			childNode.vInitX = 		thisNode.vInitX;		// based on parent's
			childNode.vInitZ = 		thisNode.vInitZ + divs;	// based on parent's + divs

			createQuadTree(childNode);

			// ----------------------------------------------------------------------------------------------- QUADRANT 2 (NE)
			nodeIndex++; 				// go to next index
			pNode->branchIndex[2] = nodeIndex;
			//cout<<"pNode->branchIndex[2]: "<<nodeIndex<<endl;

			// calculate boundary
			childNode.top = 			pNode->top;
			childNode.bottom = 	pNode->top + childNode.height;
			childNode.left = 			pNode->right - childNode.width;
			childNode.right = 		childNode.left + childNode.width;
			childNode.parentID = 	thisNode.ID;			// parent nodeIndex
			childNode.ID = 			nodeIndex;

			childNode.layerID = 		thisNode.layerID + 1;		// the next layer now
			childNode.vInitX = 		thisNode.vInitX + divs;	// based on parent's + divs
			childNode.vInitZ =		thisNode.vInitZ;			// based on parent's

			createQuadTree(childNode);

			// ----------------------------------------------------------------------------------------------- QUADRANT 3 (SE)
			nodeIndex++; 				// go to next index
			pNode->branchIndex[3] = nodeIndex;
			//cout<<"pNode->branchIndex[3]: "<<nodeIndex<<endl;

			// calculate boundary
			childNode.top = 		pNode->bottom - childNode.height;
			childNode.bottom = 	childNode.top + childNode.height;
			childNode.left = 		pNode->right - childNode.width;
			childNode.right = 		childNode.left + childNode.width;
			childNode.parentID = 	thisNode.ID;			// parent nodeIndex
			childNode.ID = 		nodeIndex;

			childNode.layerID = 		thisNode.layerID + 1;		// the next layer now
			childNode.vInitX = 		thisNode.vInitX + divs;	// based on parent's + divs
			childNode.vInitZ =		thisNode.vInitZ + divs;	// based on parent's + divs

			createQuadTree(childNode);

			//cout<<endl;
		}


		// Go to next level's nodeIndex
		//nodeIndex++;

	//}

}

void TerrainQuadTree::adjustVerticeIndex()
{
	// adjust the vertices so that it is 0 - 31 rather than 32 (inaccessible null)
	cout<<"-------------------- Adjust VerticeIndex"<<endl;
	for(int i=0; i<nodeSize; i++)
	{
		for(int x=0; x<3; x++)
		{
			for(int z=0; z<3; z++)
			{
				// cout<<"["<<qtNodeArray[i].verticeIndex[x][z].x<<" "<<qtNodeArray[i].verticeIndex[x][z].z<<"]  "<<endl;;

				if (qtNodeArray[i].verticeIndex[x][z].x > 0)
					qtNodeArray[i].verticeIndex[x][z].x = qtNodeArray[i].verticeIndex[x][z].x - 1;

				// cout<<"BEFORE:: qtNodeArray[i].verticeIndex[x][z].z "<<qtNodeArray[i].verticeIndex[x][z].z<<endl;
				if (qtNodeArray[i].verticeIndex[x][z].z > 0)
				{
					qtNodeArray[i].verticeIndex[x][z].z = qtNodeArray[i].verticeIndex[x][z].z - 1;
					// cout<<"AFTER:: qtNodeArray[i].verticeIndex[x][z].z "<<qtNodeArray[i].verticeIndex[x][z].z<<endl;
				}


			}
		}
	}
}

void TerrainQuadTree::resetNodeVisibility()
{
	for(int i=0; i<nodeSize; i++)
	{
		// clear visible state to default
		qtNodeArray[i].visible = false;

	}
}

void TerrainQuadTree::testRenderable(TERRAINQUADTREENODE &parentNode, Vector3f pos, float range)
{

	// -------------------------------------------------------------------------------- DEBUG
	/*
		unsigned int node = 64;
		float sx = qtNodeArray[node].position.x - pos.x;
		float sy = qtNodeArray[node].position.y - pos.y;
		float sz = qtNodeArray[node].position.z - pos.z;
		float sd = sqrt(	(sx*sx) + (sz*sz) );

		cout<<"node "<<node<<": "<<qtNodeArray[node].position.x<<" "<<qtNodeArray[node].position.y<<" "<<qtNodeArray[node].position.z<<" | ";

		if(sd < range)	// if the distance is close enough, set drawable
		{
			cout<<sd<<endl;
		}
		else
		{
			cout<<sd<<" out of range ("<<range<<")"<<endl;
		}
		cout<<"cam: "<<pos.x<<" "<<pos.y<<" "<<pos.z<<endl;
		cout<<qtNodeArray[node].position.x - pos.x<<" "<<qtNodeArray[node].position.y - pos.y<<" "<<qtNodeArray[node].position.z - pos.z<<endl;
	*/
	// -------------------------------------------------------------------------------- DEBUG

	if (parentNode.nodeType != QT_LEAF)	// if not leaf node, continue recursion
	{
		// if any of the child is true, the parent should not be rendered
		bool childState0 = false;
		bool childState1 = false;
		bool childState2 = false;
		bool childState3 = false;

		// ----------------->> calculate this node's four quadrants

		// @@@@@@@@@@@ FIRST child node
		//float x = qtNodeArray[parentNode.branchIndex[0]].position.x - pos.x;
		//float y = qtNodeArray[parentNode.branchIndex[0]].position.y - pos.y;
		//float z = qtNodeArray[parentNode.branchIndex[0]].position.z - pos.z;
		//float d = sqrt(	(x*x) + (z*z) );

		float TLx = qtNodeArray[parentNode.branchIndex[0]].left - pos.x;
		float TLz = qtNodeArray[parentNode.branchIndex[0]].top - pos.z;
		float TLd = sqrt(	(TLx*TLx) + (TLz*TLz) );

		float TRx = qtNodeArray[parentNode.branchIndex[0]].right - pos.x;
		float TRz = qtNodeArray[parentNode.branchIndex[0]].top - pos.z;
		float TRd = sqrt(	(TRx*TRx) + (TRz*TRz) );

		float BLx = qtNodeArray[parentNode.branchIndex[0]].left - pos.x;
		float BLz = qtNodeArray[parentNode.branchIndex[0]].bottom - pos.z;
		float BLd = sqrt(	(BLx*BLx) + (BLz*BLz) );

		float BRx = qtNodeArray[parentNode.branchIndex[0]].right - pos.x;
		float BRz = qtNodeArray[parentNode.branchIndex[0]].bottom - pos.z;
		float BRd = sqrt(	(BRx*BRx) + (BRz*BRz) );

		//if(d < range)	// if the distance is close enough, set drawable
		if((TLd < range) || (TRd < range) || (BLd < range) || (BRd < range))	// if the distance to the four corners of this node is close enough, set drawable
		{
			childState0 = true;
			qtNodeArray[parentNode.branchIndex[0]].visible = false;
			testRenderable(qtNodeArray[parentNode.branchIndex[0]], pos, range); // recurse into the child nodes

			//qtNodeArray[parentNode.branchIndex[0]].visible = true;	// is visible

		}
		else// if (d >= range)
		{
			//childState0 = false;
			qtNodeArray[parentNode.branchIndex[0]].visible = true;

			//cout<<"------->> node["<<parentNode.branchIndex[0]<<"] is visible"<<endl;
		}

		// second child node
		//x = qtNodeArray[parentNode.branchIndex[1]].position.x - pos.x;
		//y = qtNodeArray[parentNode.branchIndex[1]].position.y - pos.y;
		//z = qtNodeArray[parentNode.branchIndex[1]].position.z - pos.z;
		//d = sqrt(	(x*x) + (z*z) );

		TLx = qtNodeArray[parentNode.branchIndex[1]].left - pos.x;
		TLz = qtNodeArray[parentNode.branchIndex[1]].top - pos.z;
		TLd = sqrt(	(TLx*TLx) + (TLz*TLz) );

		TRx = qtNodeArray[parentNode.branchIndex[1]].right - pos.x;
		TRz = qtNodeArray[parentNode.branchIndex[1]].top - pos.z;
		TRd = sqrt(	(TRx*TRx) + (TRz*TRz) );

		BLx = qtNodeArray[parentNode.branchIndex[1]].left - pos.x;
		BLz = qtNodeArray[parentNode.branchIndex[1]].bottom - pos.z;
		BLd = sqrt(	(BLx*BLx) + (BLz*BLz) );

		BRx = qtNodeArray[parentNode.branchIndex[1]].right - pos.x;
		BRz = qtNodeArray[parentNode.branchIndex[1]].bottom - pos.z;
		BRd = sqrt(	(BRx*BRx) + (BRz*BRz) );

		//if(d < range)	// if the distance is close enough, set drawable
		if((TLd < range) || (TRd < range) || (BLd < range) || (BRd < range))	// if the distance to the four corners of this node is close enough, set drawable
		{
			childState1 = true;
			qtNodeArray[parentNode.branchIndex[1]].visible = false;
			testRenderable(qtNodeArray[parentNode.branchIndex[1]], pos, range); // recurse into the child nodes

			//qtNodeArray[parentNode.branchIndex[1]].visible = true;	// is visible

		}
		else// if (d >= range)
		{
			//childState1 = false;
			qtNodeArray[parentNode.branchIndex[1]].visible = true;

			//cout<<"------->> node["<<parentNode.branchIndex[1]<<"] is visible"<<endl;
		}


		// third child node
		//x = qtNodeArray[parentNode.branchIndex[2]].position.x - pos.x;
		//y = qtNodeArray[parentNode.branchIndex[2]].position.y - pos.y;
		//z = qtNodeArray[parentNode.branchIndex[2]].position.z - pos.z;
		//d = sqrt(	(x*x) + (z*z) );

		TLx = qtNodeArray[parentNode.branchIndex[2]].left - pos.x;
		TLz = qtNodeArray[parentNode.branchIndex[2]].top - pos.z;
		TLd = sqrt(	(TLx*TLx) + (TLz*TLz) );

		TRx = qtNodeArray[parentNode.branchIndex[2]].right - pos.x;
		TRz = qtNodeArray[parentNode.branchIndex[2]].top - pos.z;
		TRd = sqrt(	(TRx*TRx) + (TRz*TRz) );

		BLx = qtNodeArray[parentNode.branchIndex[2]].left - pos.x;
		BLz = qtNodeArray[parentNode.branchIndex[2]].bottom - pos.z;
		BLd = sqrt(	(BLx*BLx) + (BLz*BLz) );

		BRx = qtNodeArray[parentNode.branchIndex[2]].right - pos.x;
		BRz = qtNodeArray[parentNode.branchIndex[2]].bottom - pos.z;
		BRd = sqrt(	(BRx*BRx) + (BRz*BRz) );

		//if(d < range)	// if the distance is close enough, set drawable
		if((TLd < range) || (TRd < range) || (BLd < range) || (BRd < range))	// if the distance to the four corners of this node is close enough, set drawable
		{
			childState2 = true;
			qtNodeArray[parentNode.branchIndex[2]].visible = false;
			testRenderable(qtNodeArray[parentNode.branchIndex[2]], pos, range);	// recurse into the child nodes

			//qtNodeArray[parentNode.branchIndex[2]].visible = true;	// is visible

		}
		else// if (d >= range)
		{
			//childState2 = false;
			qtNodeArray[parentNode.branchIndex[2]].visible = true;
		}


		// fourth child node
		//x = qtNodeArray[parentNode.branchIndex[3]].position.x - pos.x;
		//y = qtNodeArray[parentNode.branchIndex[3]].position.y - pos.y;
		//z = qtNodeArray[parentNode.branchIndex[3]].position.z - pos.z;
		//d = sqrt(	(x*x) + (z*z) );

		TLx = qtNodeArray[parentNode.branchIndex[3]].left - pos.x;
		TLz = qtNodeArray[parentNode.branchIndex[3]].top - pos.z;
		TLd = sqrt(	(TLx*TLx) + (TLz*TLz) );

		TRx = qtNodeArray[parentNode.branchIndex[3]].right - pos.x;
		TRz = qtNodeArray[parentNode.branchIndex[3]].top - pos.z;
		TRd = sqrt(	(TRx*TRx) + (TRz*TRz) );

		BLx = qtNodeArray[parentNode.branchIndex[3]].left - pos.x;
		BLz = qtNodeArray[parentNode.branchIndex[3]].bottom - pos.z;
		BLd = sqrt(	(BLx*BLx) + (BLz*BLz) );

		BRx = qtNodeArray[parentNode.branchIndex[3]].right - pos.x;
		BRz = qtNodeArray[parentNode.branchIndex[3]].bottom - pos.z;
		BRd = sqrt(	(BRx*BRx) + (BRz*BRz) );

		//if(d < range)	// if the distance is close enough, set drawable
		if((TLd < range) || (TRd < range) || (BLd < range) || (BRd < range))	// if the distance to the four corners of this node is close enough, set drawable
		{
			childState3 = true;
			qtNodeArray[parentNode.branchIndex[3]].visible = false;
			testRenderable(qtNodeArray[parentNode.branchIndex[3]], pos, range);	// recurse into the child nodes
			//qtNodeArray[parentNode.branchIndex[3]].visible = true;	// is visible

		}
		else //if (d >= range)
		{
			//childState3 = false;
			qtNodeArray[parentNode.branchIndex[3]].visible = true;

			//cout<<"------->> node["<<parentNode.branchIndex[3]<<"] is visible"<<endl;
		}

		// if any of them is true (visible), the parent visibility should be false
		if ((childState0 == true) || (childState1 == true) || (childState2 == true) || (childState3 == true))
		{
			parentNode.visible = false;
		}
	}
	else
	{
		// if recursed until leaf, this node must be visible!

		parentNode.visible = true;
		//cout<<"------->> node["<<parentNode.ID<<"] is visible"<<endl;
	}

}

void TerrainQuadTree::reportNodeBranchIndex()
{
	cout<<"----------------------------->> REPORTING NODE BRANCH INDICES"<<endl;
	for(int i=0; i<nodeSize; i++)
	{
		if(qtNodeArray[i].nodeType == QT_LEAF)
		{
			//cout<<"    leaf ---->> qtNodeArray["<<i<<"]"<<endl;
		}
		else
		{
			cout<<"qtNodeArray["<<i<<"] visible:"<<qtNodeArray[i].visible<<endl;

			for(int j=0; j<4; j++)
			{
					cout<<"----------- child branchIndex["<<j<<"]:: "<<qtNodeArray[i].branchIndex[j]<<" visible: "<<qtNodeArray[qtNodeArray[i].branchIndex[j]].visible<<endl;
			}
		}

	}
}
