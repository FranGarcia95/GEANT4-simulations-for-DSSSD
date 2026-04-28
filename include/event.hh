#ifndef EVENT_HH
#define EVENT_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "run.hh"

class MyEventAction : public G4UserEventAction 
{
public:
    MyEventAction(MyRunAction*);
    ~MyEventAction();

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);
    
    
    void AddEdep_dE(G4double edep_dE){ fEdep_dE += edep_dE; }
    void AddEdep_E_frontX(G4double edep_E_frontX){ fEdep_E_frontX += edep_E_frontX; }
    void AddEdep_E_frontY(G4double edep_E_frontY){ fEdep_E_frontY += edep_E_frontY; }
    void AddEdep_E_backX(G4double edep_E_backX){ fEdep_E_backX += edep_E_backX; }
    void AddEdep_E_backY(G4double edep_E_backY){ fEdep_E_backY += edep_E_backY; }  
    
    
    void SetScatterAngle(G4double angle) {
        if (!fAngleSet) {
            fScatterAngle = angle;
            fAngleSet = true ;
        }
    }
    
    G4double GetScatterAngle() const { return fScatterAngle;}
    G4bool HasScatterAngle() const { return fAngleSet;}
    
private:
    
    G4double fEdep_dE;          
    G4double fEdep_E_frontX;    
    G4double fEdep_E_frontY;    
    G4double fEdep_E_backX;     
    G4double fEdep_E_backY;     
    
    
    G4double fScatterAngle;
    G4bool fAngleSet;
};

#endif
