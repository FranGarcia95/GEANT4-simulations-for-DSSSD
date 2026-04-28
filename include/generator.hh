#ifndef GENERATOR_HH
#define GENERATOR_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4Event.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include "Randomize.hh"
#include "G4RotationMatrix.hh"
#include "G4VUserEventInformation.hh"
#include "UserInput.hh"
#include "bifprobability.hh"
#include "PrimaryParticleInformation.hh"

class MyPrimaryGenerator : public G4VUserPrimaryGeneratorAction
{
public:
    MyPrimaryGenerator(DSSSDUserInput *u);
    ~MyPrimaryGenerator();
    
    virtual void GeneratePrimaries(G4Event*);
    
private:
    DSSSDUserInput *userInp;
    G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();

    G4double E_Incident;
    G4ParticleGun* fParticleGun;
    G4ParticleDefinition *particle;
    G4ParticleDefinition *secondaryparticle;

    G4float M_a, M_A; // mass of incident particle (neutron)
    G4double theta_lab;
    G4double Qreaction;

    G4double theta_frame;   // uniform in [-1,1]
    G4double phi;

    G4double dx, dy, dz;

    G4String AngularEmmission;

    double Kinematic_Energy(G4float m_b, G4float m_B);
    double Angle_Convertion(G4float m_b, G4float m_B, G4float theta);

    std::vector<std::pair<G4float, G4float>> bif;
    std::vector<std::pair<G4float, G4float>> angdist;

};

#endif
