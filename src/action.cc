#include "action.hh"

MyActionInitialization::MyActionInitialization(DSSSDUserInput* u)
{
	userInp = u;
}

MyActionInitialization::~MyActionInitialization()
{}


void MyActionInitialization::BuildForMaster() const
{

	MyRunAction *runAction = new MyRunAction(userInp);
	SetUserAction(runAction);
}


void MyActionInitialization::Build() const
{

	MyPrimaryGenerator *generator = new MyPrimaryGenerator(userInp);
	SetUserAction(generator);
	
	MyRunAction *runAction = new MyRunAction(userInp);
	SetUserAction(runAction);

	MyEventAction *eventAction = new MyEventAction(runAction);
	SetUserAction(eventAction);
	
	MySteppingAction *steppingAction = new MySteppingAction(eventAction);
	SetUserAction(steppingAction);
}
