#include "detector.hh"
#include <algorithm>
#include "event.hh"
#include <sstream>
#include "G4RunManager.hh"  
#include "G4AnalysisManager.hh"  

MySensitiveDetector::MySensitiveDetector(G4String name, DSSSDUserInput* u) : G4VSensitiveDetector(name), fDetectorName(name)
{
    userInp = u;

    NumDet = userInp->Get_TotalNumberDectorsDSSSD();
    G4int counter = 0;

    //Detectors ID for filling the histogram spectra
    for(int det = 0; det < NumDet; det++){
        G4int flag_DSSSD = userInp->Get_flagDSSSD(det + 1);
        if(flag_DSSSD == 1){
            HistdetID.push_back(counter);
            HistID.push_back(HistdetID);
            counter++;
            HistdetID.clear();
        }else if(flag_DSSSD == 2){
            HistdetID.push_back(counter);
            counter++;
            HistdetID.push_back(counter);
            counter++;
            HistID.push_back(HistdetID);
            HistdetID.clear();
        }
    }

    numbersideDSSSD = counter;
    counter = 0;

    for(int det = 0; det < NumDet; det++){
        G4int flag_DSSSD = userInp->Get_flagDSSSD(det + 1);
        if(flag_DSSSD == 1){
            HistdetID.push_back(counter);
            HistID_angle.push_back(HistdetID);
            counter++;
            HistdetID.clear();
        }else if(flag_DSSSD == 2){
            HistdetID.push_back(counter);
            counter++;
            HistID_angle.push_back(HistdetID);
            HistdetID.clear();
        }
    }
    
}

MySensitiveDetector::~MySensitiveDetector()
{}

void MySensitiveDetector::Initialize(G4HCofThisEvent*)
{
    edep_SSD.assign(userInp->Get_TotalNumberDectorsDSSSD(), std::vector<G4double>(16, 0.0));
    edep_DSSSD_Front.assign(userInp->Get_TotalNumberDectorsDSSSD(), std::vector<G4double>(16, 0.0));
    edep_DSSSD_Back.assign(userInp->Get_TotalNumberDectorsDSSSD(), std::vector<G4double>(16, 0.0));

    particleID_SSD.assign(userInp->Get_TotalNumberDectorsDSSSD(), std::vector<G4int>(16, 0.0));
    particleID_DSSSD_Front.assign(userInp->Get_TotalNumberDectorsDSSSD(), std::vector<G4int>(16, 0.0));
    particleID_DSSSD_Back.assign(userInp->Get_TotalNumberDectorsDSSSD(), std::vector<G4int>(16, 0.0));

    // --- Define particle codes (bit mask) ---
    proton = 1;   // 001
    triton = 2;   // 010
    alpha  = 4;   // 100

}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist)
{
    
    if (!aStep) return false;

    G4double edep = aStep->GetTotalEnergyDeposit();
    if (edep <= 0.) return false;

    const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();
    G4int stripID = touchable->GetCopyNumber(0);
    G4int DetID = touchable->GetCopyNumber(1);

    G4Track* track = aStep->GetTrack();
    const G4ParticleDefinition* particle = track->GetParticleDefinition();
    G4String particleName = particle->GetParticleName();

    int code = 0;

    // --- Assign code ---
    if(particleName == "proton") {
        code = proton;
    }  else if (particleName == "triton") {
        code = triton;
    }else if (particleName == "alpha") {
        code = alpha;
    }else {
        return false; // ignore other particles
    }
    
    //if(particleName = "proton")
    
    if(userInp->Get_flagDSSSD(DetID) == 1){
        edep_SSD[DetID - 1][stripID] += edep;
        particleID_SSD[DetID - 1][stripID] |= code;
    }else if(userInp->Get_flagDSSSD(DetID) == 2){
        G4int yIndex = (stripID % 1000) - 200;  // Y (0-15)
        G4int xIndex = (stripID / 1000) - 100;  // X (0-15)
        edep_DSSSD_Front[DetID - 1][xIndex] += edep;
        edep_DSSSD_Back[DetID - 1][yIndex] += edep;
        particleID_DSSSD_Front[DetID - 1][xIndex] |= code;
        particleID_DSSSD_Back[DetID - 1][yIndex] |= code;
    }else {
        G4cout << " Unknown Detector Name: " << fDetectorName << " with copyNo: " << stripID << G4endl;
    }

    return true;
}

void MySensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
    auto analysisManager = G4AnalysisManager::Instance();
    G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

    auto event = G4RunManager::GetRunManager()->GetCurrentEvent();
    auto primary_particle = event->GetPrimaryVertex(0)->GetPrimary(0);
    float kineticEnergy = primary_particle->GetKineticEnergy();
    const auto information =
    (PrimaryParticleInformation*)primary_particle->GetUserInformation();
    if (information) {
        const auto position = information->GetPosition();
        const auto x = position.x();
        const auto y = position.y();
        const auto z = position.z();
        analysisManager->FillH1(numbersideDSSSD + NumDet*2, x);
        analysisManager->FillH1(numbersideDSSSD + NumDet*2 + 1, y);
        analysisManager->FillH1(numbersideDSSSD + NumDet*2 + 2, z);
        const auto thetalab = information-> Getlabangle();
        const auto thetacm  = information-> GetCMangle();
        for(int det = 0; det < NumDet; det++){
            for(G4int strip = 0; strip < 16; strip++){
                if(edep_DSSSD_Front[det][strip] > 0 || edep_SSD[det][strip] > 0){
                    analysisManager->FillH1(numbersideDSSSD + NumDet*2 + 3, x);
                    analysisManager->FillH1(numbersideDSSSD + NumDet*2 + 4, y);
                    analysisManager->FillH1(numbersideDSSSD + NumDet*2 + 5, z);    
                }
                if(userInp->Get_flagDSSSD(det + 1) == 2){
                    if(edep_DSSSD_Front[det][strip] > 0){
                        analysisManager->FillH1(numbersideDSSSD + HistID_angle[det][0]*2, thetalab/deg);
                        analysisManager->FillH1(numbersideDSSSD + HistID_angle[det][0]*2 + 1, thetacm/deg);
                    }  
                }else if(userInp->Get_flagDSSSD(det + 1) == 1){
                    if(edep_SSD[det][strip] > 0){
                        analysisManager->FillH1(numbersideDSSSD + HistID_angle[det][0]*2, thetalab/deg);
                        analysisManager->FillH1(numbersideDSSSD + HistID_angle[det][0]*2 + 1, thetacm/deg);
                    }
                }
            }
        }
    }

    for (int det = 0; det < NumDet; det++) {
        G4int flag_DSSSD = userInp->Get_flagDSSSD(det + 1);
        for(G4int strip = 0; strip < 16; strip++){
            if(flag_DSSSD == 1){
                if (edep_SSD[det][strip] <= 0) continue;

                analysisManager->FillNtupleIColumn(0, 0, evt);
                analysisManager->FillNtupleIColumn(0, 1, det);
                analysisManager->FillNtupleIColumn(0, 2, strip);
                analysisManager->FillNtupleIColumn(0, 3, 0);    
                analysisManager->FillNtupleDColumn(0, 4, edep_SSD[det][strip] / CLHEP::keV);
                analysisManager->FillNtupleIColumn(0, 5, particleID_SSD[det][strip]);
                analysisManager->AddNtupleRow(0);

                analysisManager->FillH1(HistID[det][0], edep_SSD[det][strip]);

                edep_SSD[det][strip] = 0.0;  // reset here
                particleID_SSD[det][strip] = 0;
            }else if(flag_DSSSD == 2){
                if(edep_DSSSD_Front[det][strip] > 0){
                    analysisManager->FillNtupleIColumn(0, 0, evt);
                    analysisManager->FillNtupleIColumn(0, 1, det);
                    analysisManager->FillNtupleIColumn(0, 2, strip);
                    analysisManager->FillNtupleIColumn(0, 3, 1);
                    analysisManager->FillNtupleDColumn(0, 4, edep_DSSSD_Front[det][strip] / CLHEP::keV);
                    analysisManager->FillNtupleIColumn(0, 5, particleID_DSSSD_Front[det][strip]);
                    analysisManager->AddNtupleRow(0);
                    analysisManager->FillH1(HistID[det][0], edep_DSSSD_Front[det][strip]);
                    edep_DSSSD_Front[det][strip] = 0.0; // reset here
                    particleID_DSSSD_Front[det][strip] = 0;
                }
                
                if(edep_DSSSD_Back[det][strip] > 0){
                    analysisManager->FillNtupleIColumn(0, 0, evt);
                    analysisManager->FillNtupleIColumn(0, 1, det);
                    analysisManager->FillNtupleIColumn(0, 2, strip);
                    analysisManager->FillNtupleIColumn(0, 3, 2);
                    analysisManager->FillNtupleDColumn(0, 4, edep_DSSSD_Back[det][strip] / CLHEP::keV);
                    analysisManager->FillNtupleIColumn(0, 5, particleID_DSSSD_Back[det][strip]);
                    analysisManager->AddNtupleRow(0);  
                    analysisManager->FillH1(HistID[det][1], edep_DSSSD_Back[det][strip]);
                    edep_DSSSD_Back[det][strip] = 0.0; // reset here
                    particleID_DSSSD_Back[det][strip] = 0;
                }
                
            }
        }
        
    }

}
