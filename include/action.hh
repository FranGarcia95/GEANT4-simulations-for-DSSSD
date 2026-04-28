#ifndef ACTION_HH
#define ACTION_HH

#include "G4VUserActionInitialization.hh"


#include "generator.hh"
#include "run.hh"
#include "event.hh"
#include "stepping.hh"
#include "UserInput.hh"

class MyActionInitialization : public G4VUserActionInitialization
{
public:
	MyActionInitialization(DSSSDUserInput* u);
	~MyActionInitialization();
	
	virtual void Build() const;
	virtual void BuildForMaster() const;

private:

    DSSSDUserInput *userInp;
};

#endif
