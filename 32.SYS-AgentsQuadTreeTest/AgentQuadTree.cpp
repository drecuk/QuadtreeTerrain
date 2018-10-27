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
#include "AgentQuadTree.h"

using namespace std;

AgentQuadTree::AgentQuadTree()
{
	cout<<"... please use the constructor with parameters"<<endl;
}

// unsigned int _vertexX, unsigned int _vertexY
AgentQuadTree::AgentQuadTree(float terrainWidthHeight, float terrain_Scale, int maxLevels, int divideThreshold)
{
	cout<<"---------------------------------->> Creating AgentQuadTree"<<endl;

  _divideThreshold = divideThreshold;
  _maxLevels = maxLevels;

	// calculate the number of nodes for memory allocation
	nodeSize = calculateNodeSize(maxLevels);

  // adjustment for setting terrain centre at origin
  float adjFromOrig = (terrain_Scale*terrainWidthHeight)/2;

  // set terrain boundary
  float _top = -adjFromOrig;
  float _bottom = adjFromOrig;
  float _left = -adjFromOrig;
  float _right = adjFromOrig;

	// calculate minimum node boundary size
	// this is for determining when a leaf node is found, for stopping further child node creation
	minSizeOfQuad = calculateMinQuadSize(_bottom - _top, _maxLevels);

	// calculate levels log4(1024) = 5
	// log(1024)/log(4) = 5
	int levels = log(pow(_bottom - _top, 2)) / log(4);

	cout<<"AgentQuadTree adjFromOrig: "<<adjFromOrig<<endl;
	cout<<"AgentQuadTree top: "<<_top<<endl;
	cout<<"AgentQuadTree bottom: "<<_bottom<<endl;
	cout<<"AgentQuadTree left: "<<_left<<endl;
	cout<<"AgentQuadTree right: "<<_right<<endl;
	cout<<"AgentQuadTree Number of levels: "<<levels<<endl;
	cout<<"AgentQuadTree maxLevels: "<<maxLevels<<endl;
  cout<<"AgentQuadTree nodeSize: "<<nodeSize<<endl;
  cout<<"AgentQuadTree minSizeOfQuad: "<<minSizeOfQuad<<endl;

	//leafNodesIndex = nodeSize - (unsigned int)pow(4, _level-1);
	//cout<<"********************* LEAF NODES STARTING FROM INDEX:"<<leafNodesIndex<<endl<<endl;

	// allocate memory for it -- no longer used. Replaced with <vector>qtNodes
	// qtNodeArray = (AGENTQUADTREENODE*)malloc( sizeof(AGENTQUADTREENODE) * nodeSize );

	// ----------------------------------------------------------------------->> input first node (root) details
	cout<<">> Creating First AgentQuadTree Node..."<<endl;
  // declare the first node, a root node
  AGENTQUADTREENODE firstNode;

	// input boundaries
	firstNode.top = _top;
	firstNode.bottom = _bottom;
	firstNode.left = _left;
	firstNode.right = _right;

	// getting the width and height up to the QTTerrain scale
	firstNode.width = terrain_Scale*terrainWidthHeight;
	firstNode.height = terrain_Scale*terrainWidthHeight;

	// set node related IDs
	firstNode.ID = 0;
	firstNode.parentID = 0;
	firstNode.layerID = 1;				// first layer (used as 4^layerID to divide the landscape)
	firstNode.nodeType = QT_LEAF;	// all nodes are initially a leaf
	firstNode.visible = false;

	// the root node is initially a leaf node, so all child is -1
  firstNode.childIndex[0] = -1;
  firstNode.childIndex[1] = -1;
  firstNode.childIndex[2] = -1;
  firstNode.childIndex[3] = -1;

  // add the first node to the array
  qtNodes.push_back(firstNode);
	// first divide the rootnode
	divideNode(&(qtNodes[0]));

	// probablly need to divide further to solve the problem of RangeQuery
	// see documentation;
	// divideNode(&(qtNodes[1]));
	// divideNode(&(qtNodes[2]));
	// divideNode(&(qtNodes[3]));
	// divideNode(&(qtNodes[4]));
}

AgentQuadTree::~AgentQuadTree()
{
	//free(qtNodeArray);
}

unsigned int AgentQuadTree::calculateNodeSize(unsigned int _level)
{
	cout<<">> Calculating number of nodes... "<<endl;
	unsigned int numNodes = 0;

	for(int i=0; i<_level; i++)
	{
		numNodes += pow(4, i);
		cout<<"Level ["<<i<<"] total nodes now is: "<<numNodes<<endl;

	}

	cout<<"********************* TOTAL NUMBER OF NODES: "<<numNodes<<endl;
	cout<<"********************* SIZE OF EACH NODE: "<<sizeof(AGENTQUADTREENODE)<<" BYTES"<<endl;
	cout<<"********************* TOTAL SIZE OF ALL NODES: "<<sizeof(AGENTQUADTREENODE)*numNodes<<" BYTES"<<endl;

	return numNodes;
}

float AgentQuadTree::calculateMinQuadSize(float _width, unsigned int _level)
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

void AgentQuadTree::resetNodeVisibility()
{
	for(int i=0; i<nodeSize; i++)
	{
		// clear visible state to default
		qtNodes[i].visible = false;
	}
}

bool AgentQuadTree::divideNode(AGENTQUADTREENODE *thisNode)
{
  bool divideCompleted = false;
  cout<<"-----------------------------------.> DIVIDING NODE "<<thisNode->ID<<endl;

  // calculate the width and height of this node from the bounds passed in
  // note that all quadrants (child nodes) in the level has the same width and height
  int childW = thisNode->width / 2;
  int childH = thisNode->height / 2;
	cout<<"-- childW and childH: "<<childW<<endl;

	// if the division is a leaf, divide it further
  if (thisNode->nodeType == QT_LEAF)	// these child nodes are definitely the last division
  {
		cout<<"-- thisNode->ID is a QT_LEAF [DIVIDE]"<<endl;
    thisNode->nodeType = QT_NODE;	// change the parent node into a QT_NODE type

    // create a temporary qtnode that can be filled up for each quadrant and
    // added to the qtNodes arraylist
    AGENTQUADTREENODE qtnode0;

    // -------------------------- FIRST CHILD QUADRANT -. 0 NW
    nodeCount++;
    thisNode->childIndex[0] = nodeCount;		// set the child index

    qtnode0.parentID = thisNode->ID;	// parent id comes from the previous node's ID
    qtnode0.ID = 	     nodeCount;	// this node ID is the nodeIndex passed in

    qtnode0.width = 	childW;
    qtnode0.height =  childH;

    // receives boundary parameters for this node (quadrant 0)
    qtnode0.top = 		thisNode->top;
    qtnode0.bottom = 	thisNode->top + childH;
    qtnode0.left = 		thisNode->left;
    qtnode0.right = 	thisNode->left + childW;

    // calculate central axial position of this node (centre of quad boundary)
    qtnode0.position.x =	(thisNode->left + (childW / 2));
    qtnode0.position.z =	(thisNode->top + (childH / 2));

    qtnode0.nodeType = QT_LEAF;	// turn this into leaf...
    qtnode0.layerID = thisNode->layerID + 1;
		qtnode0.visible = false;

    cout<<"NW Quadrant Node 0 | parentID: "<<qtnode0.parentID<<" | ID: "<<nodeCount<<endl;
    cout<<"nodeType:: "<<qtnode0.nodeType<<"   | width:"<<qtnode0.width<<" height:"
		<<qtnode0.height<<" | top:"<<qtnode0.top<<" bottom:"<<qtnode0.bottom<<" left:"
		<<qtnode0.left<<" right: "<<qtnode0.right;
    cout<<" | central axis: "<<qtnode0.position.x<<","<<qtnode0.position.y<<","<<qtnode0.position.z<<endl;


    // -------------------------- SECOND QUADRANT -. 1 SW
    AGENTQUADTREENODE qtnode1;
    nodeCount++;
		cout<<"childIndex[1] before: "<<thisNode->childIndex[1]<<" | nodeCount:"<<nodeCount<<endl;
    thisNode->childIndex[1] = nodeCount;		// set the child index
		cout<<"childIndex[1] after: "<<thisNode->childIndex[1]<<" | nodeCount:"<<nodeCount<<endl;

    qtnode1.parentID = 	thisNode->ID;	// parent id comes from the previous node's ID
    qtnode1.ID = 	nodeCount;	// this node ID is the nodeIndex passed in

    qtnode1.width = 	childW;
    qtnode1.height = 	childH;

    // receives boundary parameters for this node (quadrant 0)
    qtnode1.top = 	thisNode->top + childH;
    qtnode1.bottom = 	thisNode->bottom;
    qtnode1.left = 	thisNode->left;
    qtnode1.right = 	thisNode->left + childW;

    // calculate central axial position of this node (centre of quad boundary)
    qtnode1.position.x =	(thisNode->left + (childW / 2));
    qtnode1.position.y =	(thisNode->bottom - (childH / 2));

    qtnode1.nodeType = QT_LEAF;	// turn this into leaf...
    qtnode1.layerID = thisNode->layerID + 1;
		qtnode1.visible = false;

    cout<<"SW Quadrant Node 1 | parentID: "<<qtnode1.parentID<<" | ID: "<<nodeCount<<endl;
    cout<<"nodeType:: "<<qtnode1.nodeType<<"   | width:"<<qtnode1.width<<" height:"<<qtnode1.height<<" | top:"<<
    qtnode1.top<<" bottom:"<<thisNode->bottom<<" left:"<<qtnode1.left<<" right:"<<qtnode1.right;
    cout<<" | central axis: "<<qtnode1.position.x<<","<<qtnode1.position.y<<","<<qtnode1.position.z<<endl;

    // -------------------------- THIRD QUADRANT -. 2 NE
    AGENTQUADTREENODE qtnode2;
    nodeCount++;
		cout<<"childIndex[2] before: "<<thisNode->childIndex[2]<<" | nodeCount:"<<nodeCount<<endl;
    thisNode->childIndex[2] = nodeCount;		// set the child index
		cout<<"childIndex[2] after: "<<thisNode->childIndex[2]<<" | nodeCount:"<<nodeCount<<endl;

    qtnode2.parentID = 	thisNode->ID;	// parent id comes from the previous node's ID
    qtnode2.ID = 		nodeCount;	// this node ID is the nodeIndex passed in

    qtnode2.width = 	childW;
    qtnode2.height = 	childH;

    // receives boundary parameters for this node (quadrant 0)
    qtnode2.top = 	thisNode->top;
    qtnode2.bottom = 	thisNode->top + childH;
    qtnode2.left = 	thisNode->right - childW;
    qtnode2.right = 	thisNode->right;

    // calculate central axial position of this node (centre of quad boundary)
    qtnode2.position.x =	(thisNode->right - (childW / 2));
    qtnode2.position.y =	(thisNode->top + (childH / 2));

    qtnode2.nodeType = QT_LEAF;	// turn this into leaf...
    qtnode2.layerID = thisNode->layerID + 1;
		qtnode2.visible = false;

    cout<<"NE Quadrant Node 2 | parentID: "<<qtnode2.parentID<<" | ID: "<<nodeCount<<endl;
    cout<<"nodeType:: "<<qtnode2.nodeType<<"   | width:"<<qtnode2.width<<" height:"
		<<qtnode2.height<<" | top:"<<qtnode2.top<<" bottom:"<<qtnode2.bottom<<" left:"
		<<qtnode2.left<<" right: "<<qtnode2.right;
    cout<<" | central axis: "<<qtnode2.position.x<<","<<qtnode2.position.y<<","<<qtnode2.position.z<<endl;


    // -------------------------- FOURTH QUADRANT -. 3 SE
    AGENTQUADTREENODE qtnode3;
    nodeCount++;
		cout<<"childIndex[3] before: "<<thisNode->childIndex[3]<<" | nodeCount:"<<nodeCount<<endl;
    thisNode->childIndex[3] = nodeCount;		// set the child index
		cout<<"childIndex[3] after: "<<thisNode->childIndex[3]<<" | nodeCount:"<<nodeCount<<endl;

    qtnode3.parentID = 	thisNode->ID;	// parent id comes from the previous node's ID
    qtnode3.ID = 		nodeCount;	// this node ID is the nodeIndex passed in

    qtnode3.width = 	childW;
    qtnode3.height = 	childH;

    // receives boundary parameters for this node (quadrant 0)
    qtnode3.top = 	thisNode->top + childH;
    qtnode3.bottom = 	thisNode->bottom;
    qtnode3.left = 	thisNode->right - childW;
    qtnode3.right = 	thisNode->right;

    // calculate central axial position of this node (centre of quad boundary)
    qtnode3.position.x =	(thisNode->right - (childW / 2));
    qtnode3.position.y =	(thisNode->bottom - (childH / 2));

    qtnode3.nodeType = QT_LEAF;	// turn this into leaf...
    qtnode3.layerID = thisNode->layerID + 1;
		qtnode3.visible = false;

    cout<<"SE Quadrant Node 3 | parentID: "<<qtnode3.parentID<<" | ID: "<<nodeCount<<endl;
    cout<<"nodeType:: "<<qtnode3.nodeType<<"   | width:"<<qtnode3.width<<" height:"
		<<qtnode3.height<<" | top:"<<qtnode3.top<<" bottom:"<<qtnode3.bottom<<" left:"
		<<qtnode3.left<<" right: "<<qtnode3.right;
    cout<<" | central axis: "<<qtnode3.position.x<<","<<qtnode3.position.y<<","<<qtnode3.position.z<<endl;

		// push all constructed nodes into the qtNodes array
		qtNodes.push_back(qtnode0);
		qtNodes.push_back(qtnode1);
    qtNodes.push_back(qtnode2);
    qtNodes.push_back(qtnode3);

    divideCompleted = true;
  }

  return divideCompleted;
}

int AgentQuadTree::getArrayIndexOfNode(int nodeID)
{
  int targetID = -1;
	cout<<"--- getArrayIndexOfNode("<<nodeID<<"): ";
  for(int i=0; i<qtNodes.size(); i++)
  {
    if(qtNodes[i].ID == nodeID)
    {
      targetID = i;
      break;
    }
  }
	cout<<targetID<<endl;
  return targetID;
}

int AgentQuadTree::placeObjectInNode(AGENTQUADTREENODE &parentNode, Object *obj)
{

  cout<<"----------------.>>> PLACEMENT FOR OBJECT["<<obj->getID()<<"] node["<<parentNode.ID<<"] node.prntID["<<parentNode.parentID<<"] ";
	cout<<" pos: "<<obj->getPosition().x<<","<<obj->getPosition().y<<","<<obj->getPosition().z<<endl;
  int nodeID = -1;

  int pID = parentNode.ID;

  // check to see if this is the last layer possible
  if(parentNode.nodeType != QT_LEAF)	// is NOT leaf go in further
  {
    cout<<"["<<parentNode.ID<<"]"<<" FIRST IF ELSE -- NOT LEAF GO DEEPER >>"<<endl;

    // which child node of the parent is this object in?
    nodeID = inWhichNodeBoundary(parentNode, obj->getPosition());

    // recurse the node where the agent is in

    AGENTQUADTREENODE tempNode = qtNodes[nodeID];

		// cout<<"** getArrayIndexOfNode("<<nodeID<<"): "<<getArrayIndexOfNode(nodeID)<<endl;

    cout<<">>>> FIRST IF ELSE recurse placeObj:"<<obj->getID()<<" into node "<<nodeID<<endl;
    placeObjectInNode(tempNode, obj);
  }
  else	// if node is currently a leaf
  {
    // calculate size of agent and environment indices
    int indexSize = parentNode.agents.size() + parentNode.emitters.size();

    cout<<"["<<parentNode.ID<<"] FIRST ELSE -- is leaf | index size: ["<<
    parentNode.agents.size()<<" + "<<parentNode.emitters.size()<<"] = "<<indexSize<<endl;

      // check if node is occupied, divideThreshold default = 1
    if (indexSize >= _divideThreshold)	// ***** if over threshold
    {
      cout<<"--["<<parentNode.ID<<"]"<<" SECOND IF ELSE -- NOT OVER THRESHOLD -- agent size: "<<parentNode.agents.size()<<" | env size: "<<parentNode.emitters.size()<<endl;
      cout<<">>>>>>>>>> total agent+env size: "<<indexSize<<" >= "<<_divideThreshold<<endl;

      if(parentNode.layerID == _maxLevels)	// and is maxLevel (no more division possible)
      {
        cout<<"----["<<parentNode.ID<<"]"<<" THIRD IF ELSE -- is maxLevel | layer ID: "<<parentNode.layerID<<endl;
        cout<<"---- ["<<parentNode.ID<<"] Objects placed in this node"<<endl;

        // decide which indices to put the object index in
        if (obj->objectType == OBJTYPE_AGENT)
        {
          cout<<"----object is stored as AGENT TYPE"<<endl;
          qtNodes[pID].agents.push_back(obj);
        }
        else if (obj->objectType == OBJTYPE_EMITTER)
        {
          cout<<"----object is stored as ENVIRONMENT TYPE"<<endl;
          qtNodes[pID].emitters.push_back(obj);
        }

          // ----------------- MAKE NODE VISIBLE WHEN THERE ARE AGENTS
          qtNodes[pID].visible = true;

          cout<<"---- ["<<parentNode.ID<<"] Agent Indices: "<<endl;
          for(int i=0; i<qtNodes[pID].agents.size(); i++)
            cout<<qtNodes[pID].agents[i]->getID()<<" "<<endl;
          cout<<endl;

          cout<<"---- ["<<parentNode.ID<<"] Environment Indices: "<<endl;
          for(int i=0; i<qtNodes[pID].emitters.size(); i++)
            cout<<qtNodes[pID].emitters[i]->getID()<<" "<<endl;
          cout<<endl;

          // store nodeID in agent
          nodeID = parentNode.ID;	// return the parent ID as the last layer
        }
        else	// is leaf, over threshold, not maxLevel (can still divide)
        {
          cout<<"---- ["<<parentNode.ID<<"] THIRD ELSE -- dividing "<<parentNode.ID<<endl;

          // %%--------------------------------------------- divide this node
          bool div = divideNode(&qtNodes[pID]);
          qtNodes[pID].nodeType = QT_NODE;

          cout<<"---- divide result: "<<div<<endl;
          //System.out.println((*parentNode).left);

          // which child node is this in?
          nodeID = inWhichNodeBoundary(qtNodes[pID], obj->getPosition());
          cout<<"---- divided new nodeID: "<<nodeID<<endl;


          // recurse this node...
          AGENTQUADTREENODE tempNode = qtNodes[nodeID];
					// cout<<"** getArrayIndexOfNode("<<nodeID<<"): "<<getArrayIndexOfNode(nodeID)<<endl;

          cout<<">>>> THIRD ELSE recurse placeObj:"<<obj->getID()<<" into node "<<nodeID<<endl;
          placeObjectInNode(tempNode, obj);
          cout<<">>>> THIRD ELSE SIMPLY PUT "<<obj->getID()<<" into "<<nodeID<<endl;

          cout<<"------------------------- agent size "<<qtNodes[pID].agents.size()<<"::"<<endl;
          for(int i=0; i < qtNodes[pID].agents.size(); i++)	// tctransfer agent indices
  			  {
  				  // add to next child node of pID
  				  //Object *wObj = qtNodes[pID].agents[i];		// get agent

  				  nodeID = inWhichNodeBoundary(qtNodes[pID], qtNodes[pID].agents[i]->getPosition()); // test agent against child of pID
  				  qtNodes[pID].agents[i]->nodeID = nodeID;
  				  qtNodes[nodeID].agents.push_back(qtNodes[pID].agents[i]);
  				  // cout<<"** getArrayIndexOfNode("<<nodeID<<"): "<<getArrayIndexOfNode(nodeID)<<endl;
  			  }
  			  cout<<endl;

  			  // ** System.out.print("------------------------- emitter size " + qtNodes.get(pID).agents.size() + "::");
  			  for(int i=0; i < qtNodes[pID].emitters.size(); i++)	// transfer agent indices
  			  {
  				// add to next child node of pID
  				  // Object *wObj = qtNodes[pID].emitters[i];		// get agent
  				  nodeID = inWhichNodeBoundary(qtNodes[pID], qtNodes[pID].emitters[i]->getPosition()); // test agent against child of pID
  				  qtNodes[pID].emitters[i]->nodeID = nodeID;
  				  qtNodes[nodeID].emitters.push_back(qtNodes[pID].emitters[i]);

  				  // ** System.out.print(qtNodes.get(pID).emitters[i].id + ", ");
  			  }
  			  cout<<endl;

  			  // clear pID agents and emitters after transfers
  			  qtNodes[pID].agents.clear();
  			  qtNodes[pID].emitters.clear();

  			  // ----------------- MAKE NODE INVISIBLE WHEN THERE ARE AGENTS
  			  qtNodes[pID].visible = false;
  			  /*
  			  for(int i=0; i < evtNodeChangeRegister.size(); i++)	// dispatch to all the listeners
  			    evtNodeChangeRegister[i].eventFunction(&info);
  			  */

          // ------------------------ transfer all agents in this node to new node ----------------
          // TrophicNetwork.transferAgent(info);	// an event replacement for c++ version
        }
      }
      else	// is leaf, not over divThreshold -- STORE THE OBJECT IN THIS NODE
      {
        cout<<"--["<<parentNode.ID<<"] SECOND ELSE -- NOT OVER THRESHOLD --"<<endl;
        cout<<"-- agents placed in node "<<parentNode.ID<<endl;

        // decide which indices to put the object index in
  			if (obj->objectType == OBJTYPE_AGENT)
  			{
  				cout<<"----object is stored as AGENT TYPE"<<endl;
  				obj->nodeID = pID; // assign this node's id to obj
  				qtNodes[pID].agents.push_back(obj);
  			}
  			else if (obj->objectType == OBJTYPE_EMITTER)
  			{
  				cout<<"----object is stored as ENVIRONMENT TYPE"<<endl;
  				obj->nodeID = pID; // assign this node's id to obj
  				qtNodes[pID].emitters.push_back(obj);
  			}

  			// ----------------- MAKE NODE VISIBLE WHEN THERE ARE AGENTS
  			qtNodes[pID].visible = true;

        nodeID = pID;	// return the parent ID as the last layer
      }
  }

  return nodeID;
}

int AgentQuadTree::inWhichNodeBoundary(AGENTQUADTREENODE parentNode, Vector3f pos)
{
  cout<<"["<<pos.x<<" "<<pos.y<<" "<<pos.z<<"]-----------------------------------.> inWhichNodeBoundary: "<<endl;
  int _childID = -1;	// -1 means no node found

	cout<<"parentNode.id: "<<parentNode.ID<<" | ";
	cout<<"parentNode.nodeType: "<<parentNode.nodeType<<" | ";
	cout<<"pos("<<pos.x<<","<<pos.z<<")"<<" | ";
  cout<<"childIndex 0: "<<parentNode.childIndex[0]<<" | ";
  cout<<"childIndex 1: "<<parentNode.childIndex[1]<<" | ";
  cout<<"childIndex 2: "<<parentNode.childIndex[2]<<" | ";
  cout<<"childIndex 3: "<<parentNode.childIndex[3]<<endl;


  if ( // NW
      (pos.z >= qtNodes[parentNode.childIndex[0]].top) &&
      (pos.z <= qtNodes[parentNode.childIndex[0]].bottom) &&
      (pos.x <= qtNodes[parentNode.childIndex[0]].right) &&
      (pos.x >= qtNodes[parentNode.childIndex[0]].left)
     )
  {
    cout<<"NW Quadrant 0 - nodeID:"<<parentNode.childIndex[0]<<" ";
    cout<<"t:"<<qtNodes[parentNode.childIndex[0]].top<<" | ";
    cout<<"b: "<<qtNodes[parentNode.childIndex[0]].bottom<<" | ";
    cout<<"l: "<<qtNodes[parentNode.childIndex[0]].left<<" | ";
    cout<<"r: "<<qtNodes[parentNode.childIndex[0]].right<<endl;

    _childID = parentNode.childIndex[0];
  } else if ( // <<endlW
      (pos.z > qtNodes[parentNode.childIndex[1]].top) &&
      (pos.z <= qtNodes[parentNode.childIndex[1]].bottom) &&
      (pos.x <= qtNodes[parentNode.childIndex[1]].right) &&
      (pos.x >= qtNodes[parentNode.childIndex[1]].left)
     )
  {
    cout<<"SW Quadrant 1 - nodeID:"<<parentNode.childIndex[1]<<" ";
		cout<<"t:"<<qtNodes[parentNode.childIndex[1]].top<<" | ";
    cout<<"b: "<<qtNodes[parentNode.childIndex[1]].bottom<<" | ";
    cout<<"l: "<<qtNodes[parentNode.childIndex[1]].left<<" | ";
    cout<<"r: "<<qtNodes[parentNode.childIndex[1]].right<<endl;

    _childID = parentNode.childIndex[1];
  } else if ( // NE
      (pos.z >= qtNodes[parentNode.childIndex[2]].top) &&
      (pos.z <= qtNodes[parentNode.childIndex[2]].bottom) &&
      (pos.x <= qtNodes[parentNode.childIndex[2]].right) &&
      (pos.x > qtNodes[parentNode.childIndex[2]].left)
     )
  {
    cout<<"NE Quadrant 2 - nodeID:"<<parentNode.childIndex[2]<<" ";
		cout<<"t:"<<qtNodes[parentNode.childIndex[2]].top<<" | ";
    cout<<"b: "<<qtNodes[parentNode.childIndex[2]].bottom<<" | ";
    cout<<"l: "<<qtNodes[parentNode.childIndex[2]].left<<" | ";
    cout<<"r: "<<qtNodes[parentNode.childIndex[2]].right<<endl;

    _childID = parentNode.childIndex[2];
  } else if (
      (pos.z > qtNodes[parentNode.childIndex[3]].top) &&
      (pos.z <= qtNodes[parentNode.childIndex[3]].bottom) &&
      (pos.x <= qtNodes[parentNode.childIndex[3]].right) &&
      (pos.x > qtNodes[parentNode.childIndex[3]].left)
     )
  {  // SE
    cout<<"SE Quadrant 3 - nodeID:"<<parentNode.childIndex[3]<<" ";
		cout<<"t:"<<qtNodes[parentNode.childIndex[3]].top<<" | ";
    cout<<"b: "<<qtNodes[parentNode.childIndex[3]].bottom<<" | ";
    cout<<"l: "<<qtNodes[parentNode.childIndex[3]].left<<" | ";
    cout<<"r: "<<qtNodes[parentNode.childIndex[3]].right<<endl;

    _childID = parentNode.childIndex[3];
  }

  // TODO
  //if (_childID == -1)
  //  System.out.println("can't find child quad to put child: " + parentNode.ID);

  return _childID;
}

bool AgentQuadTree::objectOffBoundary(Vector3f pos, float top, float bottom, float left, float right)
{
		bool isOff = false;

		if((pos.x < left) || (pos.x > right))
			isOff = true;

		if((pos.z < top) || (pos.z > bottom))
			isOff = true;

		return isOff;
}


//
// void AgentQuadTree::createQuads(AGENTQUADTREENODE thisNode)
// {
//   NODETYPE theCurrentNodeType;
//
//   // testing for leaf and assigning nodeType based on the size of the quad
//   if ((thisNode.bottom - thisNode.top) == minSizeOfQuad)
//   {	theCurrentNodeType = QT_LEAF;	}
//   else
//   {	theCurrentNodeType = QT_NODE;	}
//
//   // --------------------------------------------------------------.> CREATE THIS NODE
//   cout<<"-----------------------------------  node["<<thisNode.ID<<"] parent["<<thisNode.parentID<<"]"<<endl;
//   thisNode.nodeType = theCurrentNodeType;
//
//   // calculate the width and height of this node from the bounds passed in
//   // note that all quadrants (child nodes) in the level has the same width and height
//   thisNode.width = 	thisNode.bottom - thisNode.top;
//   thisNode.height = thisNode.right - thisNode.left;
//
//   // receives boundary parameters for this node
//   thisNode.top = 		thisNode.top;
//   thisNode.bottom = thisNode.bottom;
//   thisNode.left = 	thisNode.left;
//   thisNode.right = 	thisNode.right;
//
//   // calculate central axial position of this node (centre of quad boundary)
//   thisNode.position.x =	((thisNode.left + thisNode.right) / 2);
//   thisNode.position.z =	((thisNode.top + thisNode.bottom) / 2);
//
//   cout<<"nodeType:: "<<thisNode.nodeType<<"   | width:"<<thisNode.width<<" height:"<<
//   thisNode.height<<" | top:"<<thisNode.top<<" bottom:"<<thisNode.bottom<<" left:"<<thisNode.left<<" right:"<<thisNode.right<<endl;
//   cout<<"central position: "<<thisNode.position.x<<" "<<thisNode.position.y<<" "<<thisNode.position.z<<endl;
//
//   // --------------------------------------------------------------.> CREATE CHID NODES!
//   if(theCurrentNodeType == QT_LEAF)	// if leaf, quit
//   {
//     cout<<"A LEAF NODE -- STOP CREATING FURTHER CHILD"<<endl;
//     return;
//   }
//   else														// otherwise, create its children nodes
//   {
//     cout<<"A NODE -- CREATE FURTHER CHILD"<<endl;
//
//     // setup first
//     int childWidth = thisNode.width / 2;
//     int childHeight = thisNode.height / 2;
//
//   	//  ----------------------------------------------------------------------------------------------- QUADRANT 0 (NW)
//     //(int id, int nodeType, int parentID, int layerID, int top, int btm, int lf, int rg, int width, int height)
//     nodeIndex++; 		// go to next index
//     thisNode.childIndex[0] = nodeIndex;
// 		AGENTQUADTREENODE qtnode0;
//     //CQuadTreeNode qtnode0 = new CQuadTreeNode(nodeIndex, thisNode.ID, 0, 0, 0, 0, 0, 0, 0);
// 		cout<<"pNode.childIndex[0]: "<<nodeIndex<<endl;
//
//     // calculate boundary
//     qtnode0.top = 		thisNode.top;
//     qtnode0.bottom = 	thisNode.top + childHeight;
//     qtnode0.left = 		thisNode.left;
//     qtnode0.right = 	thisNode.left + childWidth;
//
// 		qtnode0.ID = nodeIndex;
// 		qtnode0.parentID = thisNode.ID;
//     qtnode0.layerID = thisNode.layerID+1;		// the next layer now
//
//     cout<<"pNode.branchIndex[0]: "<<nodeIndex;
//     //cout<<" | top:"<<childtop<<" bottom:"<<childbottom<<" left:"<<childleft<<" right:"<<childright<<endl;
//
//     qtNodes.push_back(qtnode0);
//
//     // ----------------------------------------------------------------------------------------------- QUADRANT 1 (SW)
//     nodeIndex++; 		// go to next index
//     thisNode.childIndex[1] = nodeIndex;
// 		AGENTQUADTREENODE qtnode1;
//     //CQuadTreeNode qtnode1 = new CQuadTreeNode(nodeIndex, thisNode.ID, 0, 0, 0, 0, 0, 0, 0);
// 		cout<<"pNode.childIndex[1]: "<<nodeIndex<<endl;
//
//     // calculate boundary
//     qtnode1.top = 		thisNode.bottom - childHeight;
//     qtnode1.bottom = 	thisNode.top + childHeight;
//     qtnode1.left = 		thisNode.left;
//     qtnode1.right = 	thisNode.left + childWidth;
//
// 		qtnode1.ID = nodeIndex;
// 		qtnode1.parentID = thisNode.ID;
//     qtnode1.layerID = thisNode.layerID+1;		// the next layer now
//
//     qtNodes.push_back(qtnode1);
//
//     // ----------------------------------------------------------------------------------------------- QUADRANT 2 (NE)
//     nodeIndex++; 				// go to next index
//     thisNode.childIndex[2] = nodeIndex;
// 		AGENTQUADTREENODE qtnode2;
// 		// CQuadTreeNode qtnode2 = new CQuadTreeNode(nodeIndex, thisNode.ID, 0, 0, 0, 0, 0, 0, 0);
//     cout<<"pNode.childIndex[2]: "<<nodeIndex<<endl;
//
//     // calculate boundary
//     qtnode2.top = 		thisNode.top;
//     qtnode2.bottom = 	thisNode.top + qtnode2.height;
//     qtnode2.left = 		thisNode.right - qtnode2.width;
//     qtnode2.right = 	qtnode2.left + qtnode2.width;
//
// 		qtnode2.ID = nodeIndex;
// 		qtnode2.parentID = thisNode.ID;
//     qtnode2.layerID = thisNode.layerID+1;		// the next layer now
//
//     qtNodes.push_back(qtnode2);
//
//     // ----------------------------------------------------------------------------------------------- QUADRANT 3 (SE)
//     nodeIndex++; 				// go to next index
//     thisNode.childIndex[3] = nodeIndex;
// 		AGENTQUADTREENODE qtnode3;
// 		// CQuadTreeNode qtnode3 = new CQuadTreeNode(nodeIndex, thisNode.ID, 0, 0, 0, 0, 0, 0, 0);
//     cout<<"pNode.childIndex[3]: "<<nodeIndex<<endl;
//
//     // calculate boundary
//     qtnode3.top = 		thisNode.bottom - qtnode3.height;
//     qtnode3.bottom = 	thisNode.top + qtnode3.height;
//     qtnode3.left = 		thisNode.right - qtnode3.width;
//     qtnode3.right = 	qtnode3.left + qtnode3.width;
//
// 		qtnode3.ID = nodeIndex;
// 		qtnode3.parentID = thisNode.ID;
//     qtnode3.layerID = thisNode.layerID+1;		// the next layer now
//
//     qtNodes.push_back(qtnode3);
//   }
// }

vector<Object*> AgentQuadTree::rangeQuery(Object *obj, OBJECTTYPE objectType ,float radius)
{
	// ArrayList<CWorldObject> rangeObjs = new ArrayList<CWorldObject>();
	vector<Object*> rangeObjs;

	cout<<"@@------------------------------------- RANGE QUERY ------->> "
										<<obj->getPosition().x<<","<<obj->getPosition().z<<endl;

	// setting up a random number generator
	int min = -1;
  int max = 2;
  srand (time(NULL));						// using time as seed
	vector<Object*>::iterator it;	// iterator
  int r = (rand()%max)-min;			// generate a number between min+1 to max

	// this chunk of code collects everything from obj's own node
	if(obj->nodeID != -1)		// if the nodeID is valid
	{
		// depending on which types, collect it
		if(obj->objectType == OBJTYPE_AGENT)	// only add this particular type to my query
		{
			cout<<"--agents only from obj->nodeID | size:  "<<qtNodes[obj->nodeID].agents.size()<<endl;
			for(int i=0; i < qtNodes[obj->nodeID].agents.size(); i++ )
			{
				cout<<" added:"<<qtNodes[obj->nodeID].agents[i]->getID()<<" from node "<<obj->nodeID<<endl;
				rangeObjs.push_back(qtNodes[obj->nodeID].agents[i]);
			}
		}

		if(obj->objectType == OBJTYPE_EMITTER)	// only add this particular type to my query
		{
			cout<<"--emitters only from obj->nodeID: "<<qtNodes[obj->nodeID].emitters.size()<<endl;
			for(int i=0; i < qtNodes[obj->nodeID].emitters.size(); i++ )
			{
				rangeObjs.push_back(qtNodes[obj->nodeID].emitters[i]);
				cout<<" added:"<<qtNodes[obj->nodeID].emitters[i]->getID()<<" from node "<<obj->nodeID<<endl;
			}
		}

		if(obj->objectType == OBJTYPE_ALL)	// if both objects
		{
			cout<<"--all only from obj->nodeID: "<<qtNodes[obj->nodeID].emitters.size()<<" + "
			<<qtNodes[obj->nodeID].emitters.size()<<endl;
			for(int i=0; i < qtNodes[obj->nodeID].agents.size(); i++ )
			{
				cout<<" added:"<<qtNodes[obj->nodeID].agents[i]->getID()<<" from node "<<obj->nodeID<<endl;
				rangeObjs.push_back(qtNodes[obj->nodeID].agents[i]);
			}
			for(int i=0; i < qtNodes[obj->nodeID].emitters.size(); i++ )
			{
				rangeObjs.push_back(qtNodes[obj->nodeID].emitters[i]);
				cout<<" added:"<<qtNodes[obj->nodeID].emitters[i]->getID()<<" from node "<<obj->nodeID<<endl;
			}
		}
	}

	// collect from all other nodes
	for(int i=0; i < qtNodes.size(); i++ )
	{
		// make sure my nodeID is not the same as the object's own node
		// already collected above
		if(qtNodes[i].ID != obj->nodeID)
		{
			// cout<<"if(qtNodes[i].ID != obj->nodeID)"<<endl;
			// only collect from leaf nodes as all objects are cascaded down to the leaves
			if(qtNodes[i].nodeType == QT_LEAF)
			{
				// cout<<" if(qtNodes[i].nodeType == QT_LEAF)"<<endl;
				// get the distance of nodes' centre with the agent's position
				float dx = qtNodes[i].position.x - obj->getPosition().x;
				float dy = qtNodes[i].position.z - obj->getPosition().z;
				float dist = sqrt((dx*dx) + (dy*dy));

				// collect indices in this node if node's centre is within radius
				if(dist <= radius)
				{
					// cout<<" if(dist <= radius)"<<endl;
					// only add this particular type to my query
					if(obj->objectType == OBJTYPE_AGENT)
					{
						// cout<<"  if(dist <= radius)"<<endl;

						// add agent indices
						if(qtNodes[i].agents.size() != 0)
						{
							cout<<"-- agents only from NODE["<<qtNodes[i].ID<<"]:: "<<endl;
							for(int j=0; j<qtNodes[i].agents.size(); j++)
							{
								int rIndex = 0;
								max = rangeObjs.size();

								if(qtNodes[i].agents.size() >= 2)
									if(rangeObjs.size() >=1)
										rIndex = (rand()%max)-min;

								// cout<<"    index: "<<rIndex<<" size:"<<rangeObjs.size()<<endl;
								it = rangeObjs.begin();
								rangeObjs.insert(it+rIndex, qtNodes[i].agents[j]);
								cout<<" added:"<<qtNodes[i].agents[j]->getID()<<" from node "<<qtNodes[i].ID<<endl;
								// rangeObjs.push_back(rIndex, qtNodes[i].agents[j]);
								//rangeObjs.add(qtNodes.get(i).agents.get(j));

								// cout<<qtNodes[i].agents[j]->getID()<<", ";
							}
						}

						cout<<endl;
					}

					if(obj->objectType == OBJTYPE_EMITTER)	// only add this particular type to my query
					{

						if(qtNodes[i].emitters.size() != 0)
						{
							cout<<"-- emitters only from NODE["<<qtNodes[i].ID<<"]:: "<<endl;
							for(int j=0; j<qtNodes[i].emitters.size(); j++)
							{
								int rIndex = 0;
								max = rangeObjs.size();

								if(qtNodes[i].emitters.size() >= 2)
									if(rangeObjs.size() >=1)
										rIndex = (rand()%max)-min;

								// cout<<"index: "<<rIndex<<" size:"<<rangeObjs.size()<<endl;
								it = rangeObjs.begin();
								rangeObjs.insert(it+rIndex, qtNodes[i].emitters[j]);
								cout<<" added:"<<qtNodes[i].emitters[j]->getID()<<" from node "<<qtNodes[i].ID<<endl;
								// rangeObjs.push_back(rIndex, qtNodes[i].emitters[j]);
							}
						}
					}

					// only add this particular type to my query
					if(obj->objectType == OBJTYPE_ALL)
					{
						if((qtNodes[i].agents.size() != 0) && (qtNodes[i].emitters.size() != 0))
							cout<<"-- ALL from NODE["<<qtNodes[i].ID<<"]:: "<<endl;

						// add agent indices
						if(qtNodes[i].agents.size() != 0)
							for(int j=0; j<qtNodes[i].agents.size(); j++)
							{
								int rIndex = 0;
								max = rangeObjs.size();

								if(qtNodes[i].agents.size() >= 2)
									if(rangeObjs.size() >=1)
										rIndex = (rand()%max)-min;

								// cout<<"index: "<<rIndex<<" size:"<<rangeObjs.size()<<endl;
								it = rangeObjs.begin();
								rangeObjs.insert(it+rIndex, qtNodes[i].agents[j]);
								cout<<" added:"<<qtNodes[i].agents[j]->getID()<<" from node "<<qtNodes[i].ID<<endl;
								// rangeObjs.push_back(rIndex, qtNodes[i].agents[j]);
							}

						if(qtNodes[i].emitters.size() != 0)
							for(int j=0; j<qtNodes[i].emitters.size(); j++)
							{
								int rIndex = 0;
								max = rangeObjs.size();

								if(qtNodes[i].emitters.size() >= 2)
									if(rangeObjs.size() >=1)
										rIndex = (rand()%max)-min;

								// cout<<"index: "<<rIndex<<" size:"<<rangeObjs.size()<<endl;
								it = rangeObjs.begin();
								rangeObjs.insert(it+rIndex, qtNodes[i].emitters[j]);
								cout<<" added:"<<qtNodes[i].emitters[j]->getID()<<" from node "<<qtNodes[i].ID<<endl;
								// rangeObjs.push_back(rIndex, qtNodes[i].emitters[j]);
							}
					}
				}
			}
		}
	}
	cout<<"@@------------------------------------- RANGE QUERY ENDS --->>"<<endl;

	return rangeObjs;
}

void AgentQuadTree::reportNodeBranchIndex()
{
	cout<<"----------------------------->> REPORTING NODE CHILD INDICES START--"<<endl;
	for(int i=0; i<qtNodes.size(); i++)
	{
		// if(qtNodes[i].nodeType == QT_NODE)
		// {
		// 	cout<<"    node ---->> qtNodes["<<i<<"]"<<endl;
		// }
		// else	// QT_LEAF
		// {
			cout<<"qtNodes["<<i<<"] visible:"<<qtNodes[i].visible<<endl;
			// report the four child nodes of each branch
			if(qtNodes[i].nodeType != QT_LEAF)
				for(int j=0; j<4; j++)
				{
						cout<<"  ---- child childIndex["<<j<<"]:: "
						<<qtNodes[i].childIndex[j]<<" visible: "
						<<qtNodes[qtNodes[i].childIndex[j]].visible<<endl;
				}
		// }
	}
	cout<<"----------------------------->> REPORTING NODE CHILD INDICES ENDS--"<<endl;
	cout<<endl;
}

void AgentQuadTree::reportQuadTree(int nodeID)
{
	cout<<"["<<nodeID<<"]--";
	if(qtNodes[nodeID].nodeType != QT_LEAF)
	{
		if(qtNodes[qtNodes[nodeID].childIndex[0]].nodeType != QT_LEAF)
			cout<<"["<<qtNodes[nodeID].childIndex[0]<<"*] ";
		else
			cout<<"["<<qtNodes[nodeID].childIndex[0]<<"] ";

		if(qtNodes[qtNodes[nodeID].childIndex[1]].nodeType != QT_LEAF)
			cout<<"["<<qtNodes[nodeID].childIndex[1]<<"*] ";
		else
			cout<<"["<<qtNodes[nodeID].childIndex[1]<<"] ";

		if(qtNodes[qtNodes[nodeID].childIndex[2]].nodeType != QT_LEAF)
			cout<<"["<<qtNodes[nodeID].childIndex[2]<<"*] ";
		else
			cout<<"["<<qtNodes[nodeID].childIndex[2]<<"] ";

		if(qtNodes[qtNodes[nodeID].childIndex[3]].nodeType != QT_LEAF)
			cout<<"["<<qtNodes[nodeID].childIndex[3]<<"*] ";
		else
			cout<<"["<<qtNodes[nodeID].childIndex[3]<<"] ";

		cout<<endl;

		// cout<<" child["<<qtNodes[nodeID].childIndex[0]<<"]--";
		if(qtNodes[qtNodes[nodeID].childIndex[0]].nodeType != QT_LEAF)
			reportQuadTree(qtNodes[nodeID].childIndex[0]);
		// cout<<" child["<<qtNodes[nodeID].childIndex[1]<<"]--";
		if(qtNodes[qtNodes[nodeID].childIndex[1]].nodeType != QT_LEAF)
			reportQuadTree(qtNodes[nodeID].childIndex[1]);
		// cout<<" child["<<qtNodes[nodeID].childIndex[2]<<"]--";
		if(qtNodes[qtNodes[nodeID].childIndex[2]].nodeType != QT_LEAF)
			reportQuadTree(qtNodes[nodeID].childIndex[2]);
		// cout<<" child["<<qtNodes[nodeID].childIndex[3]<<"]--";
		if(qtNodes[qtNodes[nodeID].childIndex[3]].nodeType != QT_LEAF)
			reportQuadTree(qtNodes[nodeID].childIndex[3]);
	}
	// cout<<endl;
}

void AgentQuadTree::printAgentIDs()
{
	cout<<endl;
	cout<<endl;
	cout<<"-------------------- printAgentIDs START--"<<endl;

	for (int i=0; i<qtNodes.size(); i++)
	{
		if(qtNodes[i].agents.size() > 0)
		{
			cout<<"node["<<i<<"]---["<<qtNodes[i].agents.size()<<" agents]--- agent IDs:";
			for (int j=0; j<qtNodes[i].agents.size(); j++)
			{
				cout<<qtNodes[i].agents[j]->getID()<<"("<<qtNodes[i].agents[j]->nodeID<<") ";
			}
			cout<<endl;
		}
	}

	for (int i=0; i<qtNodes.size(); i++)
	{
		if(qtNodes[i].emitters.size() > 0)
		{
			cout<<"node["<<i<<"]---["<<qtNodes[i].agents.size()<<" emitters]--- emitters IDs:";
			for (int j=0; j<qtNodes[i].emitters.size(); j++)
			{
				cout<<qtNodes[i].emitters[j]->getID()<<"("<<qtNodes[i].emitters[j]->nodeID<<") ";
			}
			cout<<endl;
		}
	}
	cout<<"-------------------- printAgentIDs END--"<<endl;
	cout<<endl;
	cout<<endl;
}
