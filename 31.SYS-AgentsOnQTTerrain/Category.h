//	##########################################################
//	By Eugene Ch'ng | www.complexity.io | 2018
//	Email: genechng@gmail.com
//	----------------------------------------------------------
//	A C++ Category ENUM, STRUCT utility header file
//
//	this is used for identifying agent types for
//	decision making during simulation
//
//	##########################################################

// struct Category
// {
	enum ObjectType { AGENT, EMITTER };
	enum SpeciesType { PREDATOR, PREY, SNACK };
// };


// finding Class name
// #include <typeinfo>
// cout<<typeid(animals[i]).name()<<endl;
// void printClassName()
// {
// 		cout<<endl<<"type id of object: "<<typeid(this).name()<<endl;  // this should always print Base as its not dereferenced
// 		cout<<endl<<"type id of dereferenced object (*this): "<<typeid(*this).name()<<endl;  // Why this is also printing Base?
// }
