#include <iostream>
#include "G4RunManager.hh"
#include "G4MTRunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4VModularPhysicsList.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4OpticalPhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4RadioactiveDecayPhysics.hh"
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronElasticPhysicsHP.hh"
#include "Shielding.hh"

#include "construction.hh"
#include "physics.hh"
#include "action.hh"
#include "UserInput.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrintUsage() {
  G4cerr << " Usage: " << G4endl;
  G4cerr << " NoOp [-i inputFileName ] [-t nThreads] [-r seed] " << G4endl;
  G4cerr << "   note: -t option is available only for multi-threaded mode."
         << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc, char** argv)
{
    //Argument evaluation
    if (argc > 7) {
        PrintUsage();
        return 1;
    }

    //Read the arguments
  if (argc > 7) {
    PrintUsage();
    return 1;
  }
    // Read arguments
    char *configFileName = (char *)"Error";
    G4long myseed = 345354;
    G4int nThreads = 1;
    for (G4int i = 1; i < argc; i = i + 2) {
        if (G4String(argv[i]) == "-i")
        configFileName = argv[i + 1];
        else if (G4String(argv[i]) == "-r")
        myseed = atoi(argv[i + 1]);
    #ifdef G4MULTITHREADED
        else if (G4String(argv[i]) == "-t")
        nThreads = atoi(argv[i + 1]);
    #endif
        else {
        PrintUsage();
        return 1;
        }
    }

    // Random engine
    CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
    G4Random::setTheSeed(myseed);

    //G4UIExecutive* ui = 0;

    DSSSDUserInput *myInput = new DSSSDUserInput(configFileName);

    #ifdef G4MULTITHREADED
    G4MTRunManager *runManager = new G4MTRunManager;
    runManager->SetNumberOfThreads(nThreads);
    #else
    G4RunManager *runManager = new G4RunManager;
    #endif

	
	//runManager->Initialize();
    runManager->SetUserInitialization(new MyDetectorConstruction(myInput)); //Construct detector

    G4VModularPhysicsList* physicsList = nullptr;

    if (myInput->Get_Emission_Mode() == "Decay") {

        physicsList = new Shielding(1, "HP", "");

    } else {

        auto phys = new G4VModularPhysicsList();

        phys->RegisterPhysics(new G4EmStandardPhysics_option4());
        phys->RegisterPhysics(new G4DecayPhysics());
        phys->RegisterPhysics(new G4RadioactiveDecayPhysics());
        phys->RegisterPhysics(new G4HadronPhysicsQGSP_BERT());

        physicsList = phys;
    }

    G4UImanager* UImanager = G4UImanager::GetUIpointer();
    UImanager->ApplyCommand("/run/setCuts 0.001 mm");
    runManager->SetUserInitialization(physicsList);

    runManager->SetUserInitialization(new MyActionInitialization(myInput));

    runManager->Initialize(); //moved to run.mac file

    if (myInput->Check_Visualization()) {
        // visualization manager
        // #ifdef G4VIS_USE
        G4VisManager *visManager = new G4VisExecutive;
        visManager->Initialize();
        // #endif

        // get the pointer to the User Interface manager
        G4UImanager *UI = G4UImanager::GetUIpointer();

        // define visualization and UI terminal for interactive mode
        // #ifdef G4UI_USE
        G4UIExecutive *ui = new G4UIExecutive(argc, argv);
        // #ifdef G4VIS_USE
        UI->ApplyCommand("/control/execute vis.mac");
        // #endif
        ui->SessionStart();
        delete ui;
        // #endif

        // #ifdef G4VIS_USE
        G4cout << "Deleting visManager" << G4endl;
        delete visManager;
        // #endif
    } else {
        // run events
        int nEvents;
        nEvents = myInput->Get_Nevents();
        runManager->BeamOn(nEvents);
    }
    G4cout << "Creating file " << myInput->Get_OutputFileName() << endl;
    G4cout << "Deleting things ..." << G4endl;
    delete myInput;
    delete runManager;

    G4cout << "All Done!" << G4endl;
  
    
    return 0;
}












