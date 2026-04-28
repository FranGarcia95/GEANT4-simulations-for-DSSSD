#ifndef PRIMARYPARTICLEINFORMATION_HH
#define PRIMARYPARTICLEINFORMATION_HH

#include <vector>

#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4VUserPrimaryParticleInformation.hh"
class PrimaryParticleInformation
    : public G4VUserPrimaryParticleInformation {
 public:
  PrimaryParticleInformation(const G4ThreeVector position, G4double thetalab, G4double thetacm)
      : _position(position), _theta_lab(thetalab), _theta_CM(thetacm) {};

  G4ThreeVector GetPosition() { return _position; };

  void Print() const {
    G4cout << "     Position : " << _position << " [mm] " << G4endl;
  };
  const G4double& Getlabangle() const { return _theta_lab; }
  const G4double& GetCMangle() const { return _theta_CM; }

 private:
  const G4ThreeVector _position;
  G4double _theta_lab;
  G4double _theta_CM;
};

#endif
