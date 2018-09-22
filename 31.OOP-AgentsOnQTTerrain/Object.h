//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented class
//  a non-instantiable object
//	##########################################################
#ifndef OBJECT_H
#define OBJECT_H

#include <iostream>
#include <string>
using namespace std;

class Object
{
protected:
  int id; // all objects has an id
public:
  // constructor
  Object(int _id): id(_id)
  {
    //cout<< "Object created!" << endl;
  }

  // all inherited members will have the getID() function
  int getID() { return id; }

  // virtual functions for implementation in derived classes
  // this is useful when each of your ABM species has
  // a different update/DrawObject implementation
  virtual void update () = 0;
  virtual void DrawObject(float red, float green, float blue) {}

  // destructor
  ~Object()
  {
     // cout<< "Object destroyed: " <<id<<endl;
  }
};
#endif
