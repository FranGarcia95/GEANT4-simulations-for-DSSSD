#ifndef BIFPROBABILITY_HH
#define BIFPROBABILITY_HH

#include <vector>
#include "G4ThreeVector.hh"
#include "G4Types.hh"
#include "G4SystemOfUnits.hh"
#include <iostream>
#include <fstream>
#include <cmath>

class BifProbability
{
 public:
  BifProbability();
  virtual ~BifProbability();

  std::vector<std::pair<G4float, G4float>> Get_BIF_Probability(G4String filename);
  std::vector<std::pair<G4float, G4float>> Get_Angular_Distribution(G4String filename);

  private:
  std::vector<std::pair<G4float, G4float>> bif;
  std::vector<std::pair<G4float, G4float>> ang;
  std::vector<std::vector<G4float>> angraw;

  
};

#endif
