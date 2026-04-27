#include "run.hh"

MyRunAction::MyRunAction(DSSSDUserInput* u)
{

    userInp = u;

    //fTimer = new G4Timer;
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(0);
    analysisManager->SetNtupleMerging(true);

    G4int ntupleID_SSD = analysisManager->CreateNtuple("DSSSD", "DSSSD");
    analysisManager->CreateNtupleIColumn("EventN");    
    analysisManager->CreateNtupleIColumn("det");   
    analysisManager->CreateNtupleIColumn("strip"); 
    analysisManager->CreateNtupleIColumn("Side");  //0 for SSD, 1 for Front DSSSD and 2 for Back DSSSD   
    analysisManager->CreateNtupleDColumn("edep"); 
    analysisManager->CreateNtupleIColumn("particle");       
    analysisManager->FinishNtuple();

    analysisManager->CreateNtuple("Scattering_Angle", "Scattering_Angle");
    analysisManager->CreateNtupleIColumn("EventN");      
    analysisManager->CreateNtupleDColumn("Theta_Lab");  
    analysisManager->CreateNtupleDColumn("Theta_CM"); 
    analysisManager->FinishNtuple();

    G4String framereference = userInp->Get_Reference_Frame();
    if(framereference == "LabFramefromCMFrame_Simultaneous"){
        analysisManager->CreateNtuple("Scattering_Angle secondary", "Scattering_Angle secondary");
        analysisManager->CreateNtupleIColumn("EventN");      
        analysisManager->CreateNtupleDColumn("Theta_Lab");  
        analysisManager->CreateNtupleDColumn("Theta_CM"); 
        analysisManager->FinishNtuple();
    }

    G4int Numberdetectors = userInp->Get_TotalNumberDectorsDSSSD();
    char name[1000], name1[1000];
    for(int i = 1; i < Numberdetectors + 1; i++){
        if(userInp->Get_flagDSSSD(i) == 1){
            sprintf(name, "SSD_%d", i);
            sprintf(name1, "Deposited energy SSD %d", i);
            analysisManager->CreateH1(name, name1, 10000, 0, 20.0, "MeV");
        }else if(userInp->Get_flagDSSSD(i) == 2){
            sprintf(name, "DSSSD_Front_%d", i);
            sprintf(name1, "Deposited energy DSSSD Front %d", i);
            analysisManager->CreateH1(name, name1, 10000, 0, 20.0, "MeV");
            sprintf(name, "DSSSD_Back_%d", i);
            sprintf(name1, "Deposited energy DSSSD Back %d", i);
            analysisManager->CreateH1(name, name1, 10000, 0, 20.0, "MeV");
        }
    }

    //histogram from the sample emission position
    analysisManager->CreateH2("hXYParticlePosition",
      "(X,Y) position of emission of primary particles in mm", 1200, -30, 30, 1200, -30, 30);
    analysisManager->CreateH2("hXZParticlePosition",
      "(X,Z) position of emission of primary particles in mm", 1200, -30, 30, 1200, -30, 30);
    analysisManager->CreateH2("hYZParticlePosition",
      "(Y,Z) position of emission of primary particles in mm", 1200, -30, 30, 1200, -30, 30);

    if(framereference == "LabFramefromCMFrame_Simultaneous"){
      analysisManager->CreateH2("Lab Angle Emission simultaneous",
                                "Lab Angle Emission simultaneous", 900, 0, 180, 900, 0, 180);
    }
    //Histogram related to the number of detectors
    G4int NumDet = userInp->Get_TotalNumberDectorsDSSSD();
    for(int det = 0; det < NumDet; det++){
        G4int flag_DSSSD = userInp->Get_flagDSSSD(det + 1);
        if(flag_DSSSD == 1){
            analysisManager->CreateH1(("Initial angle from the lab frame SSD " + std::to_string(det + 1)).c_str(), 
                                      ("Initial angle from the lab frame SSD " + std::to_string(det + 1)).c_str(), 180, 0, 180);
            analysisManager->CreateH1(("Initial angle from the cm frame SSD " + std::to_string(det + 1)).c_str(), 
                                      ("Initial angle from the cm frame SSD " + std::to_string(det + 1)).c_str(), 180, 0, 180);
        }else if(flag_DSSSD == 2){
            analysisManager->CreateH1(("Initial angle from the lab frame DSSSD " + std::to_string(det + 1)).c_str(), 
                                      ("Initial angle from the lab frame DSSSD " + std::to_string(det + 1)).c_str(), 180, 0, 180);
            analysisManager->CreateH1(("Initial angle from the cm frame DSSSD " + std::to_string(det + 1)).c_str(), 
                                      ("Initial angle from the cm frame DSSSD " + std::to_string(det + 1)).c_str(), 180, 0, 180);
        }
    }

    analysisManager->CreateH1("hXParticlePosition",
      "X position of emission of primary particles in mm", 1200, -30, 30);
    analysisManager->CreateH1("hYParticlePosition",
      "Y position of emission of primary particles in mm", 1200, -30, 30);
    analysisManager->CreateH1("hZParticlePosition",
      "Z position of emission of primary particles in mm", 1200, -30, 30);

    analysisManager->CreateH1("hXParticlePositionDetected",
      "X position of emission of primary particles detected in mm", 1200, -30, 30);
    analysisManager->CreateH1("hYParticlePositionDetected",
      "Y position of emission of primary particles detected in mm", 1200, -30, 30);
    analysisManager->CreateH1("hZParticlePositionDetected",
      "Z position of emission of primary particles detected in mm", 1200, -30, 30);
    
}

MyRunAction::~MyRunAction()
{}

void MyRunAction::BeginOfRunAction(const G4Run* run)
{
    auto analysisManager = G4AnalysisManager::Instance();

    analysisManager->SetVerboseLevel(1);

    G4String fileName = userInp->Get_OutputFileName();

    G4cout << "Opening output file: " << fileName << G4endl;

    analysisManager->OpenFile(fileName);
}

void MyRunAction::EndOfRunAction(const G4Run*)
{
    G4AnalysisManager *analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}
