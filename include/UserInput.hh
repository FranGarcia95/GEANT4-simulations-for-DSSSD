#ifndef DSSSDUserInput_h
#define DSSSDUserInput_h

#include <fstream>
#include <iostream>
#include <string>

#include "G4ThreeVector.hh"

using namespace std;

class DSSSDUserInput {
 public:
  DSSSDUserInput(char *config_fname);
  ~DSSSDUserInput();
  inline string Get_Target() { return targetName; }
  inline string Get_TargetShape() { return sampleshape; }
  inline string Get_Particle() { return particle; }
  inline string Get_Emission_Mode() { return emissionMode; }
  inline string Get_BIF_Filename() { return bifFilename; }
  inline string Get_Reference_Frame() { return referenceFrame; }
  inline string Get_Angular_Distribution() { return angulardistribution; }
  inline string Get_Ang_File_Name() { return AngularDistributionFileName; }
  inline string Get_Secondary_Particle() { return secondaryparticle; }
  inline float Get_Particle_Energy() { return particleEnergy; }
  inline float Get_Particle_Energy_Spread() { return energyspread; }
  inline int Get_Verbose_Level() { return verboseLevel; }
  inline double Get_Nevents() { return nEvents; }
  inline bool Check_Visualization() { return flagVisualization; }
  inline string Get_OutputFileName() { return outputFilename; }
  void Get_TargetCircular(float &diameter, float &thickness);
  void Get_TargetRectangular(float &height, float &width, float &thickness);
  void Get_Origin(float &x0, float &y0, float &z0);
  void Get_Direction(float &x0, float &y0, float &z0);
  void Return_particleinfo(float &Incident, float &Target, float &Ejected, float &Residual);
  void Get_MissAligment(float &missalignx, float &missaligny);
  void Return_DSSSD(int *theDetectorType, float* thethickness, float *thez,
                                float *thetaRot, float *phiRot, float *translationx, 
                                float *translationy, float *translationz);
  void Return_Rotation(float &targetrotationx, float &targetrotationy, float &targetrotationz);  
  int Get_TotalNumberDectorsDSSSD() { return TotalNumberDectors; }
  int Get_ZDecayIsotope() { return ZDecayIsotope; }
  int Get_ADecayIsotope() { return ADecayIsotope; }

  bool Is_DSSSD_Define(int i);
  bool Is_SSD_Define(int i);

  int Get_flagDSSSD(int Detn) { return flagDSSSD[Detn]; }

 private:
 
  void Read_ConfigFile(char *finpName);

  string targetName, particle, emissionMode, bifFilename, energyFilename,
      outputFilename, referenceFrame, sampleshape, angulardistribution,
      AngularDistributionFileName, secondaryparticle;
  float incidentparticle, targetnucleus, ejectedparticle, residualnucleus;
  bool flagEnergyDistribution, flagEnergyCascade, flagCascadeSimultaneous,
      flagnEventsFromFile, flagVisualization, flagNewNuDEX;
  int verboseLevel; 

  float targetDiameter, targetHeight, targetWidth, targetThickness, targetaMass, 
        targetDensity, particleEnergy, energyspread, Xorigin, Yorigin, Zorigin, Xdirection, 
        Ydirection, Zdirection, TargetRotationX, TargetRotationY, TargetRotationZ,
        MissalignY, MissalignX;

  int NumberDetector, TotalNumberDectors;
  int ZDecayIsotope, ADecayIsotope;
  float energyPercen = 0.0;
  int flagDSSSD[50];
  float zDSSSD[50], thetaDSSSD[50], phiDSSSD[50], ThicknessDSSSD[50];
  float TranslationX[50], TranslationY[50], TranslationZ[50];
  int EARNumber;
  double nEvents, nEventsFile;
};

#endif