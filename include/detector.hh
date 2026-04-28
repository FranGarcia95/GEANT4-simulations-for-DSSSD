#ifndef DETECTOR_HH
#define DETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4SystemOfUnits.hh"
#include "UserInput.hh"
#include "run.hh"
#include <array>
#include <vector>
#include "PrimaryParticleInformation.hh"

class MySensitiveDetector : public G4VSensitiveDetector
{
public:
    MySensitiveDetector(G4String, DSSSDUserInput *u);
    ~MySensitiveDetector() override;
    
    void Initialize(G4HCofThisEvent* hce) override;
    G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*) override;
    void EndOfEvent(G4HCofThisEvent* hce) override;
    
private:
    G4String fDetectorName;  
    
    DSSSDUserInput *userInp;

    G4int NumDet;

    std::vector<G4int> HistdetID;
    std::vector<std::vector<G4int>> HistID;
    std::vector<std::vector<G4int>> HistID_angle;
    G4int numbersideDSSSD;

    int proton, triton, alpha;   // 100

    std::vector<std::vector<G4double>> edep_SSD;
    std::vector<std::vector<G4double>> edep_DSSSD_Front;
    std::vector<std::vector<G4double>> edep_DSSSD_Back;

    std::vector<std::vector<G4int>> particleID_SSD;
    std::vector<std::vector<G4int>> particleID_DSSSD_Front;
    std::vector<std::vector<G4int>> particleID_DSSSD_Back;

};

#endif
