#include "event.hh"
#include "detector.hh"
#include "G4SystemOfUnits.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"

MyEventAction::MyEventAction(MyRunAction*)
{
    fEdep_dE = 0.;
    fEdep_E_frontX = 0.;
    fEdep_E_frontY = 0.;
    fEdep_E_backX = 0.;
    fEdep_E_backY = 0.;  
    
    fScatterAngle = -1.0;
    fAngleSet = false;
}

MyEventAction::~MyEventAction()
{}

void MyEventAction::BeginOfEventAction(const G4Event*)
{
    fEdep_dE = 0.;
    fEdep_E_frontX = 0.;
    fEdep_E_frontY = 0.;
    fEdep_E_backX = 0.;
    fEdep_E_backY = 0.;  
    
    fScatterAngle = -1.0;
    fAngleSet = false;
}

void MyEventAction::EndOfEventAction(const G4Event*)
{
    
}
