//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object class - father of all world objects
//	##########################################################
#ifndef OBJECT_H
#define OBJECT_H

#include "Abstract.h"
#include "Vector3f.h"
#include <iostream>
using namespace std;

enum OBJECTTYPE {OBJTYPE_AGENT, OBJTYPE_EMITTER, OBJTYPE_ALL};   // for determining ObjectType

class Object: public Abstract
{
private:
  bool isExpired;
protected:
  int id;         // all objects has an id
  Vector3f vPos;  // position of the object
public:
  // objectType allows differentiation between agent and emitter
  int objectType;

  // a nodeID to store which node it is in for more efficient processing
  int nodeID;

  // constructor
  Object(int _id): id(_id) {cout<<"new Object id:"<<_id<<endl;}

  // all inherited members will have the getID() function
  int getID() { return id; }
  Vector3f getPosition() {return vPos;}

  // expired objects (agents, emitters) are removed from the system
  void setExpired() {isExpired = true;};

  // virtual functions for implementation in derived classes
  // this is useful when each of your ABM species has
  // a different update/DrawObject implementation
  virtual void update() {}
  virtual void DrawObject(float red, float green, float blue) {}

  // destructor
  ~Object() {}
};

#endif
