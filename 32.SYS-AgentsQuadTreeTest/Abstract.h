//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Object Oriented BASE CLASS
//  a non-instantiable object
//	##########################################################
#ifndef ABSTRACT_H
#define ABSTRACT_H

class Abstract
{
public:
  // constructor
  Abstract() {}

  // virtual functions for implementation in derived classes
  // this is useful when each of your ABM species has
  // a different update/DrawObject implementation
  virtual void update () = 0;

  // destructor
  ~Abstract() {}
};

#endif
