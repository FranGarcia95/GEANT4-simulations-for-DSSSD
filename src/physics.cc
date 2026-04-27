#include "physics.hh"

MyPhysicsList::MyPhysicsList()

{
    RegisterPhysics (new G4EmStandardPhysics_option4()); //EM physics for Cherenkov detector
    RegisterPhysics (new G4OpticalPhysics()); //Optical photons
    RegisterPhysics (new G4DecayPhysics());
    RegisterPhysics (new G4RadioactiveDecayPhysics());
    //RegisterPhysics (new G4HadronPhysicsQGSP_BERT_HP());
}

MyPhysicsList::~MyPhysicsList()
{}
