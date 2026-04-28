#ifndef RUN_HH
#define RUN_HH

#include "G4UserRunAction.hh"
#include "G4AnalysisManager.hh"
#include "G4Run.hh"
#include "UserInput.hh"
#include <sstream>  

class MyRunAction : public G4UserRunAction{
public:
    MyRunAction(DSSSDUserInput *u);
    ~MyRunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

    G4String outputFileName;

    const std::map<int, G4int>& Get_NtupleID_SSD() const {return NtupleID_SSD;}
    const std::map<int, G4int>& Get_NtupleID_DSSSD_Front() const {return NtupleID_DSSSD_Front;}
    const std::map<int, G4int>& Get_NtupleID_DSSSD_Back() const {return NtupleID_DSSSD_Back;}

private:

    DSSSDUserInput *userInp;

    std::map<G4int, G4int> NtupleID_SSD;
    std::map<G4int, G4int> NtupleID_DSSSD_Front;
    std::map<G4int, G4int> NtupleID_DSSSD_Back;
};

#endif
